#pragma once
#include "serial.h"

#include <vector>
#include <map>
#include <string>

// �޸ľ����к�
bool change_serial_number(char drive, DWORD serial);

// ��ȡȫ��Ӳ�̾������к�
std::map<char, unsigned long> get_disk_all_drive_serial();
