#include "tppClientVariableManager.h"

#include "tppISocket.h"
#include "tppUIConnectionWindow.h"

#include <algorithm>
#include <chrono>

void tppAssert(bool condition)
{
	if (!condition)
	{
		int* i = nullptr;
		*i = 0;
	}
}

// TODO Remove ugly const_cast and just make explicit that things can be modified. It does
// mean that we have to change the code upstream
void SerializeTppVariableUpdatePacket(const tpp::VariableBase* variable, tpp::BinarySerializationWriter& stream)
{
	size_t startSize = stream.Size();

	// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
	// size that we know once the whole message has been constructed
	stream << tpp::MessageHeader(0, tpp::MessageType::Update);

	// We don't serialize the path when updating the packet, as we pass in the hash
	stream << tpp::VariableHeader(variable->type, (uint64_t)variable->m_hash);

	variable->SerializeValue(stream);

	// Patch in the packet size
	size_t totalSize = stream.Size() - startSize;
	size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

	tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(stream.Back() - totalSize + 1);
	header->size = (decltype(header->size))packetSize;
}

tpp::VariableBase* tpp::VariableDatabase::GetVariable(const std::string& path) const
{
	auto variable = m_variableHashmap.find(path);

	if (variable != m_variableHashmap.end())
	{
		return variable->second.get();
	}
	else
	{
		return nullptr;
	}
}

void tpp::VariableDatabase::AddVariable(const std::string& path, const std::shared_ptr<VariableBase>& variable)
{
	auto variableIter = m_variableHashmap.find(path);

	if (variableIter == m_variableHashmap.end())
	{
		m_variableHashmap.insert({ path, variable });
	}
}

void tpp::VariableDatabase::RemoveVariable(const std::string& path)
{
	m_variableHashmap.erase(path);
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

void tpp::VariableGroupTree::AddPath(const std::string& path, tpp::VariableGroup* variableGroup)
{
	auto groupNode = m_variableGroupNodeHashmap.find(path);

	if (groupNode != m_variableGroupNodeHashmap.end())
	{
		tppAssert(groupNode->second->variableGroup == variableGroup);
	}
	else
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

		currentNode->variableGroup = variableGroup;

		m_variableGroupNodeHashmap.insert({ path, currentNode });
	}
}

bool tpp::VariableGroupTree::Exists(const std::string& path)
{
	return m_variableGroupNodeHashmap.find(path) != m_variableGroupNodeHashmap.end();
}

const tpp::VariableGroupNode* tpp::VariableGroupTree::GetVariableGroupNode(const std::string& path) const
{
	auto variableGroupNode = m_variableGroupNodeHashmap.find(path);

	if (variableGroupNode != m_variableGroupNodeHashmap.end())
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
	m_variableGroupNodeHashmap.clear();
}

tpp::ClientVariableManager::ClientVariableManager(const char* ipAddress, uint32_t port)
	: m_windowOpen(true)
{
	m_networkAddress = tpp::NetworkAddress(ipAddress, port);

	m_clientSocket = std::unique_ptr<tpp::ISocket>(tpp::Network::CreateSocket());
	m_clientSocket->Create();
	m_clientSocket->SetBlocking(false);

	m_displayString += ipAddress;
	m_displayString += ":";
	m_displayString += std::to_string(port);
	m_displayString += "###";
	m_displayString += std::to_string((uintptr_t)this);

	m_uiConnectionWindow = std::make_unique<tpp::UIConnectionWindow>(this);

	// TODO Turn into a normal variable group, sort by favorite/non-favorite
	VariableGroup& favoriteGroup = m_favoriteGroupHashmap.insert({ "Favorites", VariableGroup() }).first->second;

	VariableGroupNode& favoriteGroupNode = m_favoriteGroupNodeHashmap.insert({ "Favorites", VariableGroupNode() }).first->second;
	favoriteGroupNode.variableGroup = &favoriteGroup;
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

	std::shared_ptr<tpp::VariableBase> variable;

	switch (variableHeader.type)
	{
		case tpp::VariableType::Float: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Float()); break;
		case tpp::VariableType::UnsignedInteger: variable = std::shared_ptr<tpp::VariableBase>(new tpp::UInt()); break;
		case tpp::VariableType::Integer: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Int()); break;
		case tpp::VariableType::Bool: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Bool()); break;
		case tpp::VariableType::Color3: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Color3()); break;
		case tpp::VariableType::Color4: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Color4()); break;
		case tpp::VariableType::Vector2: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Vector2()); break;
		case tpp::VariableType::Vector3: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Vector3()); break;
		case tpp::VariableType::Vector4: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Vector4()); break;
		case tpp::VariableType::String: variable = std::shared_ptr<tpp::VariableBase>(new tpp::String()); break;
		case tpp::VariableType::Enum: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Enum()); break;
		case tpp::VariableType::Callback: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Callback()); break;
		case tpp::VariableType::Flags8: variable = std::shared_ptr<tpp::VariableBase>(new tpp::Flags8()); break;
		default: validVariable = false;
	}

	if (validVariable)
	{
		variable->DeserializeMetadata(reader);
		variable->SetPath(path);
		variable->m_hash = variableHeader.hash;

		AddVariable(variable);
	}
	else
	{
		m_uiConnectionWindow->Log("Unrecognized variable found while processing packet\n");
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
			m_clientSocket->Create();
			m_clientSocket->SetBlocking(false);

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
			m_uiConnectionWindow->Log("Trying to connect to server... (%lli)\n", currentTime);
			tpp::SocketReturn::T connectReturn = m_clientSocket->Connect(m_networkAddress);

			if (connectReturn == SocketReturn::Ok)
			{
				m_uiConnectionWindow->Log("Connected to server %s:%i\n", m_networkAddress.address, m_networkAddress.port);
			}

			m_lastAttemptedConnection = currentTime;
		}
	}
}

void tpp::ClientVariableManager::DrawConnectionWindow()
{
	tpp::UIInteractionData interactionData;
	m_uiConnectionWindow->Draw(this, &m_windowOpen, interactionData);

	if (interactionData.editedVariable)
	{
		SerializeTppVariableUpdatePacket(interactionData.editedVariable, m_writerStream);
	}

	if (interactionData.addedToFavorites)
	{
		AddToFavorites("Favorites", interactionData.addedToFavorites);
	}
}

void tpp::ClientVariableManager::AddVariable(const std::shared_ptr<VariableBase>& variable)
{
	const std::string& groupPath = variable->m_groupPath;

	// Find variable group, or create empty if it doesn't exist
	VariableGroup* variableGroup = nullptr;
	{
		auto variableGroupIterator = m_variableGroupHashmap.find(groupPath);

		if (variableGroupIterator == m_variableGroupHashmap.end())
		{
			variableGroupIterator = m_variableGroupHashmap.insert({ groupPath, VariableGroup() }).first;
		}

		variableGroup = &variableGroupIterator->second;
	}

	m_variableDatabase.AddVariable(variable->GetPath(), variable);

	// Insert a pointer to the variable that we inserted (as a copy of the pointer)
	variableGroup->variableStrings.insert(variable->GetPath());

	// Add to the tree
	// TODO No need to do this if group existed already
	m_variableGroupTree.AddPath(groupPath, variableGroup);
}

const char* tpp::ClientVariableManager::GetDisplayString() const
{
	return m_displayString.c_str();
}

bool tpp::ClientVariableManager::IsConnected() const
{
	return m_clientSocket->IsConnected();
}

void tpp::ClientVariableManager::Clear()
{
	m_variableGroupTree.Clear();

	m_variableGroupHashmap.clear();

	m_uiConnectionWindow->Clear();
}

bool tpp::ClientVariableManager::MarkedAsClosed() const
{
	return !m_windowOpen;
}

const tpp::VariableGroup* tpp::ClientVariableManager::GetVariableGroup(const std::string& path) const
{
	auto variableGroup = m_variableGroupHashmap.find(path);

	if (variableGroup != m_variableGroupHashmap.end())
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

void tpp::ClientVariableManager::AddToFavorites(const std::string& path, tpp::VariableBase* variable)
{
	auto favoritesGroup = m_favoriteGroupHashmap.find(path);

	if (favoritesGroup != m_favoriteGroupHashmap.end())
	{
		favoritesGroup->second.variableStrings.insert(variable->GetPath());
	}
}