#include "tppServer.h"

#include "tppNetwork.h"
#include "tppISocket.h"
#include "tppSerialize.h"
#include "tppTypes.h"

#include "tppServerVariableManager.h"

tpp::ISocket* tpp::Server::GlobalServerSocket = nullptr;

tpp::ISocket* tpp::Server::GlobalClientSocket = nullptr;

bool tpp::Server::SentFullVariableTable = false;

tpp::NetworkAddress tpp::Server::GlobalNetworkAddress;

char tpp::Server::ReceiveBuffer[tpp::Server::BufferLength];

static void SerializeVariableDescription(const tpp::VariableBase* variable, const std::string& path, const tpp::VariableId& variableId, tpp::BinarySerializationWriter& writer)
{
	size_t startSize = writer.Size();

	// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
	// size that we know once the whole message has been constructed
	writer << tpp::MessageHeader(0, tpp::MessageType::Declaration);

	// Serialize the path of the variable so the server can display it
	writer << path;

	writer << tpp::VariableHeader(variable->type, variableId);

	variable->SerializeMetadata(writer);

	size_t totalSize = writer.Size() - startSize;
	size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

	tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(writer.Back() - totalSize + 1);
	header->size = (decltype(header->size))packetSize;
}

static void SerializeVariableDeletion(const tpp::VariableId& variableId, tpp::BinarySerializationWriter& writer)
{
	size_t startSize = writer.Size();

	// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
	// size that we know once the whole message has been constructed
	writer << tpp::MessageHeader(0, tpp::MessageType::Deletion);

	writer << variableId;

	size_t totalSize = writer.Size() - startSize;
	size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

	tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(writer.Back() - totalSize + 1);
	header->size = (decltype(header->size))packetSize;
}

static void SerializePendingVariableOperations(tpp::BinarySerializationWriter& variableDescriptionTable)
{
	tpp::GetServerVariableManager()->ForEachPendingOperation([&variableDescriptionTable](const tpp::Operation& operation)
	{
		if (operation.messageType == tpp::MessageType::Declaration)
		{
			const tpp::VariableDescription* variableDescription = tpp::GetServerVariableManager()->Find(operation.id);
			SerializeVariableDescription(variableDescription->variable, variableDescription->path, operation.id, variableDescriptionTable);
		}
		else if (operation.messageType == tpp::MessageType::Deletion)
		{
			SerializeVariableDeletion(operation.id, variableDescriptionTable);
		}
		else if (operation.messageType == tpp::MessageType::Update)
		{

		}
	});

	tpp::GetServerVariableManager()->ClearPendingOperations();
}

void tpp::Server::Initialize(const tpp::NetworkAddress& networkAddress)
{
	tpp::Network::Initialize();

	GlobalServerSocket = tpp::Network::CreateSocket();
	GlobalServerSocket->Create();
	GlobalServerSocket->SetBlocking(false);
	GlobalServerSocket->Listen(networkAddress.port);

	GlobalClientSocket = tpp::Network::CreateSocket();
	GlobalClientSocket->Create();
	GlobalClientSocket->SetBlocking(false);

	GlobalNetworkAddress = networkAddress;
}

void tpp::Server::Update()
{
	if (GlobalClientSocket->IsConnected())
	{
		if (!SentFullVariableTable)
		{
			tpp::GetServerVariableManager()->ResetVariableDescriptions();
			SentFullVariableTable = true;
		}

		// Send the full variable table as this is the first connection
		if (tpp::GetServerVariableManager()->HasPendingOperations())
		{
			tpp::BinarySerializationWriter VariableDescriptionTable(2048);
			SerializePendingVariableOperations(VariableDescriptionTable);
			GlobalClientSocket->Send(VariableDescriptionTable.Data(), VariableDescriptionTable.Size());
		}

		tpp::SocketReturn::T receiveResult = GlobalClientSocket->Receive(ReceiveBuffer, BufferLength);

		// If we have received valid data
		if (receiveResult > 0)
		{
			// Copy all the data into the serialization stream
			// TODO Use the original buffer instead
			tpp::BinarySerializationReader reader(ReceiveBuffer, receiveResult);

			while (reader.HasData())
			{
				tpp::MessageHeader messageHeader;
				reader << messageHeader;

				tpp::VariableHeader variableHeader;
				reader << variableHeader;

				const tpp::VariableDescription* variableDescription = tpp::GetServerVariableManager()->Find(variableHeader.id);

				if (variableDescription)
				{
					tpp::VariableBase* variable = variableDescription->variable;

					if (variable)
					{
						variable->DeserializeValue(reader);
					}
				}
			}
		}
		else if (receiveResult == tpp::SocketReturn::Ok || receiveResult == tpp::SocketReturn::WouldBlock)
		{
			// Ignore
		}
		else if (receiveResult == tpp::SocketReturn::ConnectionClosed)
		{
			// Close the socket. This means we've closed the server
			GlobalClientSocket->Close();
			printf("Connection closed\n");

			// Reopen and leave in a good state
			GlobalClientSocket->Create();
			GlobalClientSocket->SetBlocking(false);
		}
		else if (receiveResult < 0)
		{
			//wchar_t errorString[256] = {};
			//int size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), 0, (LPWSTR)&errorString, sizeof(errorString), NULL);
			//
			//printf("recv failed with error: %S\n", errorString);
		}
	}
	else
	{
		// TODO Try to connect every heartbeat to avoid spamming

		tpp::SocketReturn::T acceptReturn = GlobalServerSocket->Accept(GlobalNetworkAddress, GlobalClientSocket);

		if (acceptReturn != tpp::SocketReturn::Ok)
		{
			GlobalServerSocket->Close();
			SentFullVariableTable = false;
		}
	}
}

void tpp::Server::Shutdown()
{
	tpp::Network::Shutdown();
}
