/*
** ͬ��WinHttp����
** author
**   fengbangyue@gmail.com
*/
#ifndef ULT_WINHTTPREQUEST_H_
#define ULT_WINHTTPREQUEST_H_

#include "./net/winhttp-foundation.h"

namespace ult {

class WinHttpRequest {

public:

  WinHttpRequest(void) {
    buffer_ = new char[kBufferLength_];
  }

  ~WinHttpRequest(void) {
    if (buffer_ != NULL) {
      delete[] buffer_;
    }
  }

  void Close(void) {
    req_.Close();
  }

  HINTERNET GetHandle(void) const {
    return req_.GetHandle();
  }

  HRESULT Request(const std::wstring& url, const std::wstring& headers = L"",
      LPCWSTR referer = WINHTTP_NO_REFERER) {
    RETURN_IF_FAILED(sion_.Initialize(false));
    URL_COMPONENTS uc;
    RETURN_IF_FAILED(ult::UltWinHttpCrackUrl(url.c_str(), &uc));
    std::wstring host_name(uc.lpszHostName, uc.dwHostNameLength);
    RETURN_IF_FAILED(conn_.Initialize(sion_, host_name.c_str(), uc.nPort));
    DWORD flag = (uc.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;

    if (!req_.Attach(::WinHttpOpenRequest(conn_, L"GET", uc.lpszUrlPath, NULL, referer,
      WINHTTP_DEFAULT_ACCEPT_TYPES, flag))) {
        return HRESULT_FROM_WIN32(::GetLastError());
    }
    DWORD feature = WINHTTP_DISABLE_COOKIES;
    ::WinHttpSetOption(req_, WINHTTP_OPTION_DISABLE_FEATURE, &feature, sizeof (feature));
    if (FALSE == ::WinHttpSendRequest(req_, headers.c_str(), -1L, WINHTTP_NO_REQUEST_DATA, 0, 0, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    if (FALSE == ::WinHttpReceiveResponse(req_, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT QueryHeaders(DWORD info_level, LPCWSTR name, LPVOID buffer,
      LPDWORD buffer_len, LPDWORD index = WINHTTP_NO_HEADER_INDEX) {
    if (FALSE == ::WinHttpQueryHeaders(req_, info_level, name, buffer, buffer_len, index)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT GetRawHeaders(std::wstring* raw_headers) {
    DWORD len = kBufferLength_;
    RETURN_IF_FAILED(QueryHeaders(WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
        buffer_, &len));
    raw_headers->assign((LPCWSTR)buffer_, len);
    return S_OK;
  }

  HRESULT GetStatusCode(LPDWORD code) {
    DWORD len = sizeof (*code);
    if (FALSE == QueryHeaders(WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, code, &len)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT ReadData(LPVOID* buffer, LPDWORD length) {
    if (FALSE == ::WinHttpReadData(req_, buffer_, kBufferLength_, length)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    *buffer = buffer_;
    return S_OK;
  }

private:

  WinHttpConnection conn_;
  WinHttpSession sion_;
  WinHttpHandle req_;

  void* buffer_;

  enum {
    kBufferLength_ = 8 * 1024,
  };

}; //class SyncWinHttpRequest
} //namespace ult
#endif