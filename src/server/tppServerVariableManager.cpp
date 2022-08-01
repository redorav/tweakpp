#include "tppServerVariableManager.h"

#include "tppISocket.h"

#include <algorithm>
#include <chrono>

tpp::VariableGroupNode* tpp::VariableGroupNode::AddFindNode(const std::string& path, const std::string& nodeName)
{
	auto it = nodes.find(nodeName);

	if (it != nodes.end())
	{
		return &it->second;
	}
	else
	{
		auto it2 = nodes.insert({ nodeName, VariableGroupNode(path) });
		return &it2.first->second;
	}
}

void tpp::VariableGroupTree::AddPath(const std::string& path)
{
	if (!Exists(path))
	{
		VariableGroupNode* currentNode = &m_root;

		size_t currentSlashPosition = 0;
		size_t newSlashPosition = path.find("/");

		// If there are no slashes, add the whole path
		if (newSlashPosition == std::string::npos)
		{
			currentNode = currentNode->AddFindNode(path, path);
		}
		else
		{
			while (currentSlashPosition != std::string::npos)
			{
				std::string nodeName = path.substr(currentSlashPosition, newSlashPosition - currentSlashPosition);
				std::string partialPath = path.substr(0, newSlashPosition);

				if (nodeName.size() > 0)
				{
					currentNode = currentNode->AddFindNode(partialPath, nodeName);
				}

				currentSlashPosition = newSlashPosition == std::string::npos ? std::string::npos : newSlashPosition + 1;
				newSlashPosition = path.find("/", currentSlashPosition);
			}
		}

		m_variableGroupNodeHashMap.insert({ path, currentNode });
	}
}

bool tpp::VariableGroupTree::Exists(const std::string& path)
{
	return m_variableGroupNodeHashMap.find(path) != m_variableGroupNodeHashMap.end();
}

const tpp::VariableGroupNode* tpp::VariableGroupTree::GetVariableGroupNode(const std::string& path) const
{
	auto variableGroupNode = m_variableGroupNodeHashMap.find(path);

	if (variableGroupNode != m_variableGroupNodeHashMap.end())
	{
		return variableGroupNode->second;
	}
	else
	{
		return nullptr;
	}
}

void tpp::VariableGroupTree::Clear()
{
	m_root = VariableGroupNode("");
	m_variableGroupNodeHashMap.clear();
}

tpp::ServerVariableManager::ServerVariableManager(const char* ipAddress, uint32_t port)
{
	m_ipAddress = ipAddress;
	m_port = port;

	m_serverSocket = tpp::Network::CreateSocket();
	m_clientSocket = tpp::Network::CreateSocket();

	m_networkAddress = tpp::NetworkAddress(m_ipAddress.c_str(), m_port);
	m_serverSocket->Create();
	m_serverSocket->SetBlocking(false);
	m_serverSocket->Listen(m_networkAddress.port);

	m_displayString = m_ipAddress;
	m_displayString += ":";
	m_displayString += std::to_string(m_port);
}

static const int DEFAULT_BUFLEN = 512;

void tpp::ServerVariableManager::ProcessDeclarationPacket(const std::vector<char>& currentPacketData)
{
	auto currentPosition = currentPacketData.begin();

	// Search for path in packet and read path
	std::string path;
	{
		auto nullTerminator = std::find(currentPosition + sizeof(tpp::MessageHeader), currentPacketData.end(), '\0');
		path = std::string(currentPacketData.begin() + sizeof(tpp::MessageHeader), nullTerminator);
		currentPosition = nullTerminator + 1;
	}

	auto valueIndex = currentPosition - currentPacketData.begin();
	const tpp::VariableHeader* variablePacket = reinterpret_cast<const tpp::VariableHeader*>(&currentPacketData[valueIndex]);

	auto variableIndex = valueIndex + sizeof(tpp::VariableHeader);

	bool validVariable = true;

	tpp::Variable variable(variablePacket->type, path, variablePacket->hash);

	switch (variablePacket->type)
	{
	case tpp::VariableType::Float:           variable.vdFloat = *reinterpret_cast<const tpp::Float*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::UnsignedInteger: variable.vdUInt = *reinterpret_cast<const tpp::UInt*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Integer:         variable.vdInt = *reinterpret_cast<const tpp::Int*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Bool:            variable.vdBool = *reinterpret_cast<const tpp::Bool*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Vector2:         variable.vdVector2 = *reinterpret_cast<const tpp::Vector2*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Vector3:         variable.vdVector3 = *reinterpret_cast<const tpp::Vector3*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Vector4:         variable.vdVector4 = *reinterpret_cast<const tpp::Vector4*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Color3:          variable.vdColor3 = *reinterpret_cast<const tpp::Color3*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Color4:          variable.vdColor4 = *reinterpret_cast<const tpp::Color4*>(&currentPacketData[variableIndex]); break;
	case tpp::VariableType::Callback: break;
	default: validVariable = false;
	}

	if (validVariable)
	{
		AddVariable(variable);
	}
	else
	{
		printf("Unrecognized variable found while processing packet\n");
	}
}

void tpp::ServerVariableManager::UpdateConnection()
{
	char receiveBuffer[DEFAULT_BUFLEN];
	int receiveBufferLength = DEFAULT_BUFLEN;

	tpp::Archive<tpp::SerializationStreamType::RawStreamWrite> serializationWriter(m_writerStream);

	if (m_clientSocket->IsConnected())
	{
		tpp::SocketReturn::T receiveResult = m_clientSocket->Receive(receiveBuffer, receiveBufferLength);

		if (receiveResult > 0)
		{
			// Parse variable description message

			// Copy all the data into a vector
			// TODO Use the original buffer instead
			m_receivedData.clear();
			m_receivedData.insert(m_receivedData.begin(), receiveBuffer, receiveBuffer + receiveResult);

			auto headerPosition = m_receivedData.begin();
			size_t offset = 0;

			// If we have a pending packet, assume the rest of the content is coming next
			// We search for another header in the incoming data. If we find one, we only
			// copy up to that point. Otherwise, copy all the data. If the incoming data
			// fills the buffer we need to wait 
			if (m_currentPacketData.size() > 0)
			{
				headerPosition = std::search(m_receivedData.begin() + offset, m_receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

				// If we actually manage to find a new header, we know where the existing message ends
				if (headerPosition != m_receivedData.end())
				{
					m_currentPacketData.insert(m_currentPacketData.end(), m_receivedData.begin(), headerPosition);
					ProcessDeclarationPacket(m_currentPacketData);
				}
			}

			while (true)
			{
				headerPosition = std::search(m_receivedData.begin() + offset, m_receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

				if (headerPosition != m_receivedData.end())
				{
					// If we found the header, there are several possible cases
					size_t remainingDataInBuffer = (size_t)receiveResult - (headerPosition - m_receivedData.begin());

					if (remainingDataInBuffer >= sizeof(tpp::MessageHeader))
					{
						tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(&*headerPosition);
						tpp::MessageType messageType = header->type;
						tpp::Version version = header->version;
						uint32_t packetSize = header->size;

						if (remainingDataInBuffer >= sizeof(tpp::MessageHeader) + packetSize)
						{
							m_currentPacketData.clear();
							m_currentPacketData.insert(m_currentPacketData.end(), headerPosition, headerPosition + sizeof(tpp::MessageHeader) + packetSize);
							ProcessDeclarationPacket(m_currentPacketData);
							offset = (headerPosition - m_receivedData.begin()) + sizeof(tpp::MessageHeader) + packetSize;
						}
						else
						{
							// Insert into the current packet, but don't process it yet as it's incomplete
							m_currentPacketData.clear();
							m_currentPacketData.insert(m_currentPacketData.end(), headerPosition, m_receivedData.end());
							break;
						}
					}
					else
					{
						// Insert into the current packet, but don't process it yet as it's incomplete
						m_currentPacketData.clear();
						m_currentPacketData.insert(m_currentPacketData.end(), headerPosition, m_receivedData.end());
						break;
					}
				}
				else
				{
					// If we didn't find the header, there are two possible reasons.
					// 1) We actually parsed all messages that arrived
					// 2) The message was split and the pattern 'tpp' has been split as well (bad luck!) so we need to store
					// whatever part is there and try again with more data

					// Insert into the current packet, but don't process it yet as it's incomplete
					m_currentPacketData.clear();
					m_currentPacketData.insert(m_currentPacketData.end(), headerPosition, m_receivedData.end());
					break;
				}
			}
		}
		else if (receiveResult == tpp::SocketReturn::Timeout || receiveResult == tpp::SocketReturn::WouldBlock)
		{

		}
		else
		{
			m_clientSocket->Close();

			// TODO Clear everything related to a connection
			Clear();
		}

		// TODO Check if it's still connected as we might have dropped the connection here!

		if (!serializationWriter.GetStream().Empty())
		{
			// TODO Handle send issues
			tpp::SocketReturn::T sendResult = m_clientSocket->Send(serializationWriter.GetStream().Data(), serializationWriter.GetStream().Size());
			serializationWriter.Clear();
		}
	}
	else
	{
		time_t currentTime = time(NULL);

		// TODO Use better time
		uint64_t timeDelta = currentTime - m_lastAttemptedConnection;

		if (timeDelta > 0)
		{
			//uiLog.Log("Listening for connections... (%lli)\n", currentTime);
			printf("Listening for connections... (%lli)\n", currentTime);
			tpp::SocketReturn::T acceptReturn = m_serverSocket->Accept(m_networkAddress, m_clientSocket);

			if (acceptReturn != tpp::SocketReturn::Ok)
			{
				m_serverSocket->Close();
			}

			m_lastAttemptedConnection = currentTime;
		}
	}
}

void tpp::ServerVariableManager::AddVariable(const Variable& variable)
{
	// TODO Make this string_view and function take a string_view
	std::string groupPath = std::string(variable.groupPath);

	m_variableGroupTree.AddPath(groupPath);

	VariableGroup* variableGroup = nullptr;

	auto variableGroupIterator = m_variableGroupHashMap.find(groupPath);

	if (variableGroupIterator == m_variableGroupHashMap.end())
	{
		variableGroupIterator = m_variableGroupHashMap.insert({ groupPath, VariableGroup() }).first;
	}

	variableGroup = &variableGroupIterator->second;

	Variable& insertedVariable = m_variableHashMap.insert({ variable.path, variable }).first->second;

	// Insert a pointer to the variable that we inserted (as a copy)
	variableGroup->variables.push_back(&insertedVariable);
}

void tpp::ServerVariableManager::NotifyVariableModified(const tpp::Variable& modifiedVariable)
{
	tpp::Archive<tpp::SerializationStreamType::RawStreamWrite> serializationWriter(m_writerStream);
	serializationWriter.SerializeTppVariableUpdatePacket(modifiedVariable);
}

const tpp::Variable* tpp::ServerVariableManager::GetVariable(const std::string& path) const
{
	auto variable = m_variableHashMap.find(path);

	if (variable != m_variableHashMap.end())
	{
		return &variable->second;
	}
	else
	{
		return nullptr;
	}
}

const char* tpp::ServerVariableManager::GetDisplayString() const
{
	return m_displayString.c_str();
}

void tpp::ServerVariableManager::Clear()
{
	m_variableGroupTree.Clear();

	m_variableGroupHashMap.clear();

	m_variableHashMap.clear();
}

const tpp::VariableGroup* tpp::ServerVariableManager::GetVariableGroup(const std::string& path) const
{
	auto variableGroup = m_variableGroupHashMap.find(path);

	if (variableGroup != m_variableGroupHashMap.end())
	{
		return &variableGroup->second;
	}
	else
	{
		return nullptr;
	}
}

const tpp::VariableGroupNode* tpp::ServerVariableManager::GetVariableGroupNode(const std::string& path) const
{
	return m_variableGroupTree.GetVariableGroupNode(path);
}
