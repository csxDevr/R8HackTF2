#pragma once

class CNetChan;
class bf_write;

int __fastcall Hooked_SendDatagram(CNetChan* netchan, void*, bf_write* datagram);
void UpdateIncomingSequences();
void ResetIncomingSequences();