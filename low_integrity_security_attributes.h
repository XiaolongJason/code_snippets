#ifndef _LOW_INTEGRITY_SECURITY_ATTRIBUTES_H_
#define _LOW_INTEGRITY_SECURITY_ATTRIBUTES_H_

//Understanding and Working in Protected Mode Internet Explorer
//http://msdn.microsoft.com/en-us/library/bb250462%28v=vs.85%29.aspx
//
// example:
// LowIntegritySecurityAttributes pipeSA;
//  ::CreateNamedPipeA(pipe_name_.c_str()
//  , PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED
//  , PIPE_TYPE_BYTE | PIPE_READMODE_BYTE
//  , PIPE_UNLIMITED_INSTANCES
//  , (DWORD)buffer_size_
//  , (DWORD)buffer_size_
//  , NMPWAIT_USE_DEFAULT_WAIT
//  , pipeSA.GetSecurityAttributes()
//  );

#include <Sddl.h>
#include "windows.h"

class LowIntegritySecurityAttributes
{
public:
  LowIntegritySecurityAttributes() 
    : security_descriptor_ptr_(NULL), 
      error_(FALSE)
  {
    error_ = CreateLowSecurityAttributes(&security_attributes_, &security_descriptor_) ? FALSE : TRUE;
  }

  ~LowIntegritySecurityAttributes()
  {
    if (security_descriptor_ptr_ != NULL)
    {
      ::LocalFree(security_descriptor_ptr_);
      security_descriptor_ptr_ = NULL;
    }
  }

  LPSECURITY_ATTRIBUTES GetSecurityAttributes()
  {
    if (error_)
      return NULL;
    return &security_attributes_;
  }
  operator LPSECURITY_ATTRIBUTES ()
  {
    return GetSecurityAttributes();
  }

protected:
  // ------------------------------------------------------------------------
  // ����		: CreateALowSA
  // ����		: ����һ����Ȩ�޵İ�ȫ������
  // ����ֵ	: BOOL 
  // ����		: SECURITY_ATTRIBUTES *pSA
  // ����		: SECURITY_DESCRIPTOR *pSecurityD
  // ��ע		: 
  // -------------------------------------------------------------------------
  BOOL LowIntegritySecurityAttributes::CreateLowSecurityAttributes (
                                    SECURITY_ATTRIBUTES *pSA, 
                                    SECURITY_DESCRIPTOR *pSecurityD
                                    )
  {
    //#define SDDL_MANDATORY_LABEL        TEXT("ML") // Integrity label
    //#define SDDL_NO_WRITE_UP          TEXT("NW")
    //#define SDDL_ML_LOW          TEXT("LW")
    // LABEL_SECURITY_INFORMATION  SDDL  SACL����Ϊ�������Լ���
    #define LOW_INTEGRITY_SDDL_SACL TEXT("S:(ML;;NW;;;LW)")

    if (security_descriptor_ptr_)
    {
      ::LocalFree(security_descriptor_ptr_);
      security_descriptor_ptr_ = NULL;
    }

    // ��ʼ����ȫ����
    BOOL ret = FALSE;

    // �����vista�򴴽�һ����Ȩ�޵Ĺ����ڴ�
    pSA->nLength = sizeof(SECURITY_ATTRIBUTES);
    pSA->bInheritHandle = FALSE;
    pSA->lpSecurityDescriptor = pSecurityD;
    do
    {
      // ��ʼ��һ����ȫ���������ڶ�����������ΪSECURITY_DESCRIPTOR_REVISION
      ret = ::InitializeSecurityDescriptor(pSA->lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
      if (!ret)
      {
        break;
      }

      // ����DACL�����ɷ��ʿ��Ʊ����˴�������һ���յ�dacl�����Ҳ�ʹ��Ĭ�ϵ�dacl
      ret = ::SetSecurityDescriptorDacl(pSA->lpSecurityDescriptor, TRUE, 0, FALSE);
      if (!ret)
      {
        break;
      }

      // ��һ����ȫ�������ַ���ת���ɰ�ȫ����������ȫ�������ַ����Ǹ���ʽ�����ַ���
      // �ڶ��������㶨ΪSDDL_REVISION_1
      // ���ĸ������Ƿ��ذ�ȫ�������ֽ���������Ϊ��
      // ���API�泤��
      ret = ::ConvertStringSecurityDescriptorToSecurityDescriptor(
        LOW_INTEGRITY_SDDL_SACL,
        SDDL_REVISION_1,
        &security_descriptor_ptr_, 
        NULL);	// xp�����api��ʧ�ܡ�
      if (!ret)
      {
        ret = TRUE;
        break;
      }

      PACL pSacl = NULL;
      BOOL fSaclPresent = FALSE;
      BOOL fSaclDefaulted = FALSE;

      // ����SACL��ϵͳ���ʿ����б�
      ret = ::GetSecurityDescriptorSacl(security_descriptor_ptr_, &fSaclPresent, &pSacl, &fSaclDefaulted);
      if (!ret)
      {
        break;
      }
      ret = ::SetSecurityDescriptorSacl(pSA->lpSecurityDescriptor, TRUE, pSacl, FALSE);
      if (!ret)
      {
        break;
      }

      ret = TRUE;

    } while (0);

    return ret;
  }
private:
  BOOL error_;
  SECURITY_ATTRIBUTES security_attributes_;
  SECURITY_DESCRIPTOR security_descriptor_;
  PSECURITY_DESCRIPTOR security_descriptor_ptr_;
};


#endif //_LOW_INTEGRITY_SECURITY_ATTRIBUTES_H_