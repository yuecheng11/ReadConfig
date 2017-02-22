#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 数据类型重定义
//typedef unsigned char   UINT8;
//typedef signed   int    INT32;
//typedef unsigned int    UINT32;

// 员工信息结构体定义
typedef struct
{
    char  szEmployeeName[128];    // 员工姓名
    int  iEmployeeAge;           // 员工年龄
} T_EmployeeInfo;

// 雇主信息结构体定义
typedef struct
{
    char  szEmployerName[128];    // 雇主姓名
    int  iEmployerAge;           // 雇主年龄
} T_EmployerInfo;

// 函数声明
void GetCompletePath(const char *pszConfigFileName, char *pszWholePath);
void GetStringContentValue(FILE *fp, const char *pszSectionName, const char *pszKeyName, char *pszOutput, int iOutputLen);
void GetConfigFileStringValue(const char *pszSectionName, const char *pszKeyName, char *pDefaultVal, char *pszOutput, int iOutputLen, char *pszConfigFileName);
int GetConfigFileIntValue(const char *pszSectionName, const char *pszKeyName, int iDefaultVal, char *pszConfigFileName);

int main()
{
    T_EmployeeInfo tEmployeeInfo = {0};
    T_EmployerInfo tEmployerInfo = {0};

    // 获取并打印员工信息
    // 获取员工姓名
    GetConfigFileStringValue("EMPLOYEEINFO", "EmployeeName", "", tEmployeeInfo.szEmployeeName, sizeof(tEmployeeInfo.szEmployeeName), "Config.ini");

    // 获取员工年龄
    tEmployeeInfo.iEmployeeAge = GetConfigFileIntValue("EMPLOYEEINFO", "EmployeeAge", 20, "Config.ini");
    if (tEmployeeInfo.iEmployeeAge == -1)  // 判断获取到的年龄是否正确
    {
        printf("Get EmployeeAge failed!\n");
        return -1;
    }

    // 打印读取到的员工姓名和年龄
    printf("EmployeeName is %s, EmployeeAge is %d\n", tEmployeeInfo.szEmployeeName, tEmployeeInfo.iEmployeeAge);

    // 获取并打印雇主信息
    // 获取雇主姓名
    GetConfigFileStringValue("EMPLOYERINFO", "EmployerName", "", tEmployerInfo.szEmployerName, sizeof(tEmployerInfo.szEmployerName), "Config.ini");

    // 获取员工年龄
    tEmployerInfo.iEmployerAge = GetConfigFileIntValue("EMPLOYERINFO", "EmployerAge", 30, "Config.ini");
    if (tEmployerInfo.iEmployerAge == -1)  // 判断获取到的年龄是否正确
    {
        printf("Get EmployerAge failed!\n");
        return -1;
    }

    // 打印读取到的员工姓名和年龄
    printf("EmployerName is %s, EmployerAge is %d\n", tEmployerInfo.szEmployerName, tEmployerInfo.iEmployerAge);

    return 0;              
}


void GetCompletePath(const char *pszConfigFileName, char *pszWholePath)

{
    char *pszHomePath      = NULL;
    char  szWholePath[256] = {0};

    // 先对输入参数进行异常判断
    if (pszConfigFileName == NULL || pszWholePath == NULL)
    {
        printf("GetCompletePath: input parameter(s) is NULL!\n");
        return;
    }

    pszHomePath = (char *)getenv("HOME");     // 获取当前用户所在的路径
    if (pszHomePath == NULL)
    {
        printf("GetCompletePath: Can't find home path!\n");
        return;
    }

    // 拼装配置文件路径
    snprintf(szWholePath, sizeof(szWholePath)-1, "%s/gitproject/ReadConfig/%s", pszHomePath, pszConfigFileName);

    strncpy(pszWholePath, szWholePath, strlen(szWholePath));
}


void GetStringContentValue(FILE *fp, const char *pszSectionName, const char *pszKeyName, char *pszOutput, int iOutputLen)
{
    char  szSectionName[100]    = {0};
    char  szKeyName[100]        = {0};
    char  szContentLine[256]    = {0};
    char  szContentLineBak[256] = {0};
    int iContentLineLen       = 0;
    int iPositionFlag         = 0;

    // 先对输入参数进行异常判断
    if (fp == NULL || pszSectionName == NULL || pszKeyName == NULL || pszOutput == NULL)
    {
        printf("GetStringContentValue: input parameter(s) is NULL!\n");
        return;
    }

    sprintf(szSectionName, "[%s]", pszSectionName);
    strcpy(szKeyName, pszKeyName);

    while (feof(fp) == 0)
    {
        memset(szContentLine, 0x00, sizeof(szContentLine));
        fgets(szContentLine, sizeof(szContentLine), fp);      // 获取段名

        // 判断是否是注释行(以;开头的行就是注释行)或以其他特殊字符开头的行
        if (szContentLine[0] == ';' || szContentLine[0] == '\r' || szContentLine[0] == '\n' || szContentLine[0] == '\0')
        {
            continue;
        }

        // 匹配段名
        if (strncasecmp(szSectionName, szContentLine, strlen(szSectionName)) == 0)     
        {
            while (feof(fp) == 0)
            {
                memset(szContentLine,    0x00, sizeof(szContentLine));
                memset(szContentLineBak, 0x00, sizeof(szContentLineBak));
                fgets(szContentLine, sizeof(szContentLine), fp);     // 获取字段值

                // 判断是否是注释行(以;开头的行就是注释行)
                if (szContentLine[0] == ';')
                {
                    continue;
                }

                memcpy(szContentLineBak, szContentLine, strlen(szContentLine));

                // 匹配配置项名
                if (strncasecmp(szKeyName, szContentLineBak, strlen(szKeyName)) == 0)     
                {
                    iContentLineLen = strlen(szContentLine);
                    for (iPositionFlag = strlen(szKeyName); iPositionFlag <= iContentLineLen; iPositionFlag ++)
                    {
                        if (szContentLine[iPositionFlag] == ' ')
                        {
                            continue;
                        }
                        if (szContentLine[iPositionFlag] == '=')
                        {
                            break;
                        }

                        iPositionFlag = iContentLineLen + 1;
                        break;
                    }

                    iPositionFlag = iPositionFlag + 1;    // 跳过=的位置

                    if (iPositionFlag > iContentLineLen)
                    {
                        continue;
                    }

                    memset(szContentLine, 0x00, sizeof(szContentLine));
                    strcpy(szContentLine, szContentLineBak + iPositionFlag);

                    // 去掉内容中的无关字符
                    for (iPositionFlag = 0; iPositionFlag < strlen(szContentLine); iPositionFlag ++)
                    {
                        if (szContentLine[iPositionFlag] == '\r' || szContentLine[iPositionFlag] == '\n' || szContentLine[iPositionFlag] == '\0')
                        {
                            szContentLine[iPositionFlag] = '\0';
                            break;
                        }
                    }

                    // 将配置项内容拷贝到输出缓存中
                    strncpy(pszOutput, szContentLine, iOutputLen-1);
                    break;
                }
                else if (szContentLine[0] == '[')
                {
                    break;
                }
            }
            break;
        }
    }
}


void GetConfigFileStringValue(const char *pszSectionName, const char *pszKeyName, char *pDefaultVal, char *pszOutput, int iOutputLen, char *pszConfigFileName)
{
    FILE  *fp                    = NULL;
    char  szWholePath[256]      = {0};

    // 先对输入参数进行异常判断
    if (pszSectionName == NULL || pszKeyName == NULL || pszOutput == NULL || pszConfigFileName == NULL)
    {
        printf("GetConfigFileStringValue: input parameter(s) is NULL!\n");
        return;
    }

    // 获取默认值
    if (pDefaultVal == NULL)
    {
        strcpy(pszOutput, "");
    }
    else
    {
        strcpy(pszOutput, pDefaultVal);
    }

    // 打开配置文件
    GetCompletePath(pszConfigFileName, szWholePath);
    fp = fopen(szWholePath, "r");
    if (fp == NULL)
    {
        printf("GetConfigFileStringValue: open %s failed!\n", szWholePath);
        return;
    }

    // 调用函数用于获取具体配置项的值
    GetStringContentValue(fp, pszSectionName, pszKeyName, pszOutput, iOutputLen);

    // 关闭文件
    fclose(fp);
    fp = NULL;
}

int GetConfigFileIntValue(const char *pszSectionName, const char *pszKeyName, int iDefaultVal, char *pszConfigFileName)
{
    char  szGetValue[512] = {0};
    int  iGetValue       = 0;

    // 先对输入参数进行异常判断
    if (pszSectionName == NULL || pszKeyName == NULL || pszConfigFileName == NULL)
    {
        printf("GetConfigFileIntValue: input parameter(s) is NULL!\n");
        return -1;
    }

    GetConfigFileStringValue(pszSectionName, pszKeyName, NULL, szGetValue, 512-1, pszConfigFileName);    // 先将获取的值存放在字符型缓存中

    if (szGetValue[0] == '\0' || szGetValue[0] == ';')    // 如果是结束符或分号, 则使用默认值
    {
        iGetValue = iDefaultVal;
    }
    else
    {
        iGetValue = atoi(szGetValue);
    }

    return iGetValue;
}

