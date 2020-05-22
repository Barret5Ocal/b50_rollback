struct network_data
{
    SOCKET in;
    SOCKET out;
    sockaddr_in Server;
    
    int Sport;
    int Rport;
    
    fd_set Master;
    
    sockaddr_in Client;
    int ClientLength;
};

int Setup(network_data *Data, int Sport, int Rport, ui_data *UIData = 0)
{
    
    Data->Sport = Sport;
    Data->Rport = Rport;
    
    
    WSADATA SData; 
    WORD Version = MAKEWORD(2, 2);
    int Wsok = WSAStartup(Version, &SData);
    
    if(Wsok != 0)
    {
        UIData->Console.AddLog("Can't start Winsock! %d\n", Wsok);
        //printf("Can't start Winsock! %d\n", Wsok);
        return 1;
    }
    
    if(!Data->Sport && !Data->Rport)
    {
        UIData->Console.AddLog("No port\n");
        //printf("No port\n");
        return 1;
    }
    
    //sockaddr_in Server;
    Data->Server.sin_family = AF_INET;
    Data->Server.sin_port = htons(Data->Sport);
    
    inet_pton(AF_INET, "127.0.0.1", &Data->Server.sin_addr);
    
    Data->out = socket(AF_INET, SOCK_DGRAM, 0);
    
    Data->in = socket(AF_INET, SOCK_DGRAM, 0);
    
    sockaddr_in ServerHint; 
    ServerHint.sin_addr.S_un.S_addr = ADDR_ANY;
    ServerHint.sin_family = AF_INET;
    ServerHint.sin_port = htons(Data->Rport);
    
    
    if(bind(Data->in, (sockaddr*)&ServerHint, sizeof(ServerHint)) == SOCKET_ERROR)
    {
        UIData->Console.AddLog("Can't bind socket! %d\n", WSAGetLastError());
        //printf("Can't bind socket! %d\n", WSAGetLastError());
        return 1;
    }
    
    
    
    FD_ZERO(&Data->Master);
    FD_SET(Data->in, &Data->Master);
    
    ZeroMemory(&Data->Client, sizeof(Data->Client));
    Data->ClientLength = sizeof(Data->Client);
    
    
    return 0;
}

int Send(char* Sbuff, network_data *Data, ui_data *UIData = 0)
{
    
    int sendOk = 0;
    
    sendOk = sendto(Data->out, Sbuff, strlen(Sbuff) + 1, 0, (sockaddr*)&Data->Server, sizeof(Data->Server));
    
    if(sendOk == SOCKET_ERROR)
    {
        UIData->Console.AddLog("That didn't work! %d\n", WSAGetLastError());
        
        //printf("That didn't work! %d\n", WSAGetLastError());
    }
    
    
    return sendOk; 
}

int Recieve(char* out, network_data *Data)
{
    
    fd_set Copy = Data->Master; 
    
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    // NOTE(Barret5Ocal): You always need to set the timeout variable. That's what tells select to not just sit in a loop
    int SocketCount = select(Data->in+1, &Copy, nullptr, nullptr, &timeout);
    
    int bytesIn = 0;
    for(int i = 0; i < SocketCount; i++)
    {
        SOCKET sock = Copy.fd_array[i];
        
        
        if(sock)
        {
            ZeroMemory(out, 1024);
            
            bytesIn = recvfrom(Data->in, out, 1024, 0, (sockaddr*)&Data->Client, &Data->ClientLength);
        }
    }
    
    return bytesIn;
}