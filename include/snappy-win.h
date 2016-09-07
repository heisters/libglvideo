// Handle missing ssize_t on Windows. Typedef's taken from: 
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751%28v=vs.85%29.aspx#LONG_PTR 
#if defined(_WIN32) || defined(_WIN64) 
#  if defined(_WIN64) 
typedef __int64 LONG_PTR;
#  else 
typedef long LONG_PTR;
#  endif 
typedef LONG_PTR SSIZE_T;
typedef SSIZE_T ssize_t;
#endif 