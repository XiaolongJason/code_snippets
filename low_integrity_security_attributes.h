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
  // 函数		: CreateALowSA
  // 功能		: 创建一个低权限的安全属性体
  // 返回值	: BOOL 
  // 参数		: SECURITY_ATTRIBUTES *pSA
  // 参数		: SECURITY_DESCRIPTOR *pSecurityD
  // 附注		: 
  // -------------------------------------------------------------------------
  BOOL LowIntegritySecurityAttributes::CreateLowSecurityAttributes (
                                    SECURITY_ATTRIBUTES *pSA, 
                                    SECURITY_DESCRIPTOR *pSecurityD
                                    )
  {
    //#define SDDL_MANDATORY_LABEL        TEXT("ML") // Integrity label
    //#define SDDL_NO_WRITE_UP          TEXT("NW")
    //#define SDDL_ML_LOW          TEXT("LW")
    // LABEL_SECURITY_INFORMATION  SDDL  SACL被设为低完整性级别
    #define LOW_INTEGRITY_SDDL_SACL TEXT("S:(ML;;NW;;;LW)")

    if (security_descriptor_ptr_)
    {
      ::LocalFree(security_descriptor_ptr_);
      security_descriptor_ptr_ = NULL;
    }

    // 初始化安全属性
    BOOL ret = FALSE;

    // 如果是vista则创建一个低权限的共享内存
    pSA->nLength = sizeof(SECURITY_ATTRIBUTES);
    pSA->bInheritHandle = FALSE;
    pSA->lpSecurityDescriptor = pSecurityD;
    do
    {
      // 初始化一个安全描述符，第二个参数必须为SECURITY_DESCRIPTOR_REVISION
      ret = ::InitializeSecurityDescriptor(pSA->lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
      if (!ret)
      {
        break;
      }

      // 设置DACL（自由访问控制表），此处设置了一个空的dacl，并且不使用默认的dacl
      ret = ::SetSecurityDescriptorDacl(pSA->lpSecurityDescriptor, TRUE, 0, FALSE);
      if (!ret)
      {
        break;
      }

      // 将一个安全描述符字符串转换成安全描述符，安全描述符字符串是个格式化的字符串
      // 第二个参数恒定为SDDL_REVISION_1
      // 第四个参数是返回安全描述符字节数，可以为空
      // 这个API真长啊
      ret = ::ConvertStringSecurityDescriptorToSecurityDescriptor(
        LOW_INTEGRITY_SDDL_SACL,
        SDDL_REVISION_1,
        &security_descriptor_ptr_, 
        NULL);	// xp下这个api会失败。
      if (!ret)
      {
        ret = TRUE;
        break;
      }

      PACL pSacl = NULL;
      BOOL fSaclPresent = FALSE;
      BOOL fSaclDefaulted = FALSE;

      // 设置SACL（系统访问控制列表）
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