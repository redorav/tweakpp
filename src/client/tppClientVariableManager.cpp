#include "tppClientVariableManager.h"

#include "tppISocket.h"
#include "tppUIConnectionWindow.h"

#include <algorithm>
#include <chrono>

template<typename TppVariable>
void SerializeTppVariableUpdate(TppVariable& variable, const tpp::Hash& hash, tpp::BinarySerializationWriter& stream)
{
	// If we're updating, we only need to send the current value
	stream << tpp::VariableHeader(variable.type, sizeof(variable.currentValue), (uint64_t)hash);
	stream << variable.currentValue;
}

template<>
void SerializeTppVariableUpdate<tpp::Callback>(tpp::Callback& variable, const tpp::Hash& hash, tpp::BinarySerializationWriter& stream)
{
	stream << tpp::VariableHeader(variable.type, 0, hash);
}

// TODO Remove ugly const_cast and just make explicit that things can be modified. It does
// mean that we have to change the code upstream
void SerializeTppVariableUpdatePacket(const tpp::Variable& inVariable, tpp::BinarySerializationWriter& stream)
{
	tpp::Variable& variable = const_cast<tpp::Variable&>(inVariable);

	size_t startSize = stream.Size();

	// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
	// size that we know once the whole message has been constructed
	stream << tpp::MessageHeader(0, tpp::MessageType::Update);

	// We don't serialize the path when updating the packet, as we pass in the hash

	if (variable.type == tpp::VariableType::Float)
	{
		SerializeTppVariableUpdate(variable.vdFloat, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::UnsignedInteger)
	{
		SerializeTppVariableUpdate(variable.vdInt, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Integer)
	{
		SerializeTppVariableUpdate(variable.vdInt, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Bool)
	{
		SerializeTppVariableUpdate(variable.vdBool, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Color3)
	{
		SerializeTppVariableUpdate(variable.vdColor3, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Color4)
	{
		SerializeTppVariableUpdate(variable.vdColor4, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Vector2)
	{
		SerializeTppVariableUpdate(variable.vdVector2, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Vector3)
	{
		SerializeTppVariableUpdate(variable.vdVector3, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Vector4)
	{
		SerializeTppVariableUpdate(variable.vdVector4, variable.hash, stream);
	}
	else if (variable.type == tpp::VariableType::Callback)
	{
		SerializeTppVariableUpdate(variable.vdCallback, variable.hash, stream);
	}
	else
	{
		printf("Variable %s not serialized correctly\n", variable.path.c_str());
	}

	// Patch in the packet size
	size_t totalSize = stream.Size() - startSize;
	size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

	tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(stream.Back() - totalSize + 1);
	header->size = (decltype(header->size))packetSize;
}

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

tpp::ClientVariableManager::ClientVariableManager(const char* ipAddress, uint32_t port)
	: m_windowOpen(true)
{
	m_serverSocket = tpp::Network::CreateSocket();
	m_clientSocket = tpp::Network::CreateSocket();

	m_networkAddress = tpp::NetworkAddress(ipAddress, port);
	m_serverSocket->Create();
	m_serverSocket->SetBlocking(false);
	m_serverSocket->Listen(m_networkAddress.port);

	m_displayString += ipAddress;
	m_displayString += ":";
	m_displayString += std::to_string(port);
	m_displayString += "##";
	m_displayString += std::to_string((uintptr_t)this);

	m_uiConnectionWindow = new tpp::UIConnectionWindow();
}

tpp::ClientVariableManager::~ClientVariableManager()
{
	delete m_uiConnectionWindow;
}

static const int DEFAULT_BUFLEN = 512;

void tpp::ClientVariableManager::ProcessDeclarationPacket(const std::vector<char>& currentPacketData)
{
	tpp::BinarySerializationReader reader(currentPacketData);

	// Search for path in packet and read path
	tpp::MessageHeader messageHeader;
	reader << messageHeader;

	std::string path;
	reader << path;

	tpp::VariableHeader variableHeader;
	reader << variableHeader;

	bool validVariable = true;

	tpp::Variable variable(variableHeader.type, path, variableHeader.hash);

	switch (variableHeader.type)
	{
		case tpp::VariableType::Float:           variable.vdFloat.DeserializeMetadata(reader); break;
		case tpp::VariableType::UnsignedInteger: variable.vdUInt.DeserializeMetadata(reader); break;
		case tpp::VariableType::Integer:         variable.vdInt.DeserializeMetadata(reader); break;
		case tpp::VariableType::Bool:            variable.vdBool.DeserializeMetadata(reader); break;
		case tpp::VariableType::Color3:          variable.vdColor3.DeserializeMetadata(reader); break;
		case tpp::VariableType::Color4:          variable.vdColor4.DeserializeMetadata(reader); break;
		case tpp::VariableType::Vector2:         variable.vdVector2.DeserializeMetadata(reader); break;
		case tpp::VariableType::Vector3:         variable.vdVector3.DeserializeMetadata(reader); break;
		case tpp::VariableType::Vector4:         variable.vdVector4.DeserializeMetadata(reader); break;
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

void tpp::ClientVariableManager::UpdateConnection()
{
	char receiveBuffer[DEFAULT_BUFLEN];
	int receiveBufferLength = DEFAULT_BUFLEN;

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

		if (!m_writerStream.Empty())
		{
			// TODO Handle send issues
			tpp::SocketReturn::T sendResult = m_clientSocket->Send(m_writerStream.Data(), m_writerStream.Size());
			m_writerStream.Clear();
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

void tpp::ClientVariableManager::DrawConnectionWindow()
{
	const tpp::Variable* modifiedVariable = nullptr;
	m_uiConnectionWindow->Draw(this, m_displayString.c_str(), &m_windowOpen, modifiedVariable);

	if (modifiedVariable)
	{
		SerializeTppVariableUpdatePacket(*modifiedVariable, m_writerStream);
	}
}

void tpp::ClientVariableManager::AddVariable(const Variable& variable)
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

const tpp::Variable* tpp::ClientVariableManager::GetVariable(const std::string& path) const
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

const char* tpp::ClientVariableManager::GetDisplayString() const
{
	return m_displayString.c_str();
}

void tpp::ClientVariableManager::Clear()
{
	m_variableGroupTree.Clear();

	m_variableGroupHashMap.clear();

	m_variableHashMap.clear();
}

bool tpp::ClientVariableManager::MarkedAsClosed() const
{
	return !m_windowOpen;
}

const tpp::VariableGroup* tpp::ClientVariableManager::GetVariableGroup(const std::string& path) const
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

const tpp::VariableGroupNode* tpp::ClientVariableManager::GetVariableGroupNode(const std::string& path) const
{
	return m_variableGroupTree.GetVariableGroupNode(path);
}
