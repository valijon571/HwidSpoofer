#pragma once
#include <Windows.h>

#include <iostream>

/* ��װ���� */
bool install_driver(const wchar_t* drvPath, const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,												// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,												// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);		// ����Ȩ��
	if (schSCManager == NULL)
	{
		std::cout << "[*] ������ : [ " << GetLastError() << " ] " << std::endl;
		CloseServiceHandle(schSCManager);
		return false;
	}

	// �����������������������ƹ��������ݿ�
	SC_HANDLE schService = CreateServiceW(
		schSCManager,									// ����ؼ����������ݿ�ľ��
		serviceName,										// Ҫ��װ�ķ��������
		serviceName,										// �û��������������ʶ�������ʾ����
		SERVICE_ALL_ACCESS,						// �Է���ķ���Ȩ�ޣ�����ȫȨ��
		SERVICE_KERNEL_DRIVER,				// �������ͣ���������
		SERVICE_DEMAND_START,				// ��������ѡ����̵��� StartService ʱ����
		SERVICE_ERROR_IGNORE,					// ����޷����������Դ����������
		drvPath,												// �����ļ�����·������������ո���Ҫ���˫����
		NULL,													// ���������ĸ��ض����飺��������ĳ����
		NULL,													// ���ն�����Ψһ���ֵ��������
		NULL,													// �������˳�����飺����û��������
		NULL,													// ���з�����˻�����ʹ�� LocalSystem �˻�
		NULL);													// LocalSystem �˻�����
	if (schService == NULL)
	{
		std::cout << "[*] ������ : [ " << GetLastError() << " ] " << std::endl;
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return false;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}

/* �������� */
bool start_driver(const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,													// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,													// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);			// ����Ȩ��
	if (schSCManager == NULL)
	{
		std::cout << "[*] ������ : [ " << GetLastError() << " ] " << std::endl;
		CloseServiceHandle(schSCManager);
		return false;
	}

	// �򿪷���
	SC_HANDLE hs = OpenServiceW(
		schSCManager,						// ����ؼ����������ݿ�ľ��
		serviceName,							// Ҫ�򿪵ķ�����
		SERVICE_ALL_ACCESS);			// �������Ȩ�ޣ�����Ȩ��
	if (hs == NULL)
	{
		std::cout << "[*] ������ : [ " << GetLastError() << " ] " << std::endl;
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return false;
	}

	// ��ʼ����
	if (StartServiceW(hs, 0, 0) == 0)
	{
		std::cout << "[*] ������ : [ " << GetLastError() << " ] " << std::endl;
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return false;
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return true;
}

/* ֹͣ���� */
bool stop_driver(const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,												// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,												// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);		// ����Ȩ��
	if (schSCManager == NULL)
	{
		CloseServiceHandle(schSCManager);
		return false;
	}

	// �򿪷���
	SC_HANDLE hs = OpenServiceW(
		schSCManager,							// ����ؼ����������ݿ�ľ��
		serviceName,								// Ҫ�򿪵ķ�����
		SERVICE_ALL_ACCESS);				// �������Ȩ�ޣ�����Ȩ��
	if (hs == NULL)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return false;
	}

	// ���������������
	SERVICE_STATUS status;
	if (QueryServiceStatus(hs, &status) == 0)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return false;
	}

	if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
	{
		// ���͹رշ�������
		if (ControlService(
			hs,												// ������
			SERVICE_CONTROL_STOP,       // �����룺֪ͨ����Ӧ��ֹͣ
			&status										// �������µķ���״̬��Ϣ
		) == 0)
		{
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return false;
		}

		// �жϳ�ʱ
		int timeOut = 0;
		while (status.dwCurrentState != SERVICE_STOPPED)
		{
			timeOut++;
			QueryServiceStatus(hs, &status);
			Sleep(50);
		}

		if (timeOut > 80)
		{
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return false;
		}
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return true;
}

/* ж������ */
bool unload_driver(const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,												// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,												// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);		// ����Ȩ��
	if (schSCManager == NULL)
	{
		CloseServiceHandle(schSCManager);
		return false;
	}

	// �򿪷���
	SC_HANDLE hs = OpenServiceW(
		schSCManager,						// ����ؼ����������ݿ�ľ��
		serviceName,							// Ҫ�򿪵ķ�����
		SERVICE_ALL_ACCESS);			// �������Ȩ�ޣ�����Ȩ��
	if (hs == NULL)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return false;
	}

	// ɾ������
	if (DeleteService(hs) == 0)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return false;
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

/* ��ʼ��װ */
bool start_install_driver(const wchar_t* drvPath, const wchar_t* serviceName, bool uninstall = false)
{
	// �ȳ���ж��
	if (uninstall)
	{
		stop_driver(serviceName);
		unload_driver(serviceName);
	}

	// ���Լ���
	return install_driver(drvPath, serviceName) && start_driver(serviceName);
}