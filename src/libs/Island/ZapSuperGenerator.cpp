#include "Island.h"

bool ISLAND::DoZapSuperGeneratorDecodeFile(char *sname)
{
    HANDLE hFile = fio->_CreateFile(sname, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        TGA_H tga_head;

        fio->_ReadFile(hFile, &tga_head, sizeof(tga_head), 0);
        dword dwSize = tga_head.width;
        byte *pTempMap = NEW byte[dwSize * dwSize];
        fio->_ReadFile(hFile, pTempMap, dwSize * dwSize, 0);
        fio->_CloseHandle(hFile);

        mzShadow.DoZip(pTempMap, dwSize);
        mzShadow.Save(string(sname) + ".zap");
        SE_DELETE(pTempMap);
    }

    return true;
}

void ISLAND::DoZapSuperGeneratorInnerDecodeFiles(char *sub_dir, char *mask)
{
    char file_mask[256];
    WIN32_FIND_DATA wfd;

    sprintf(file_mask, "resource\\foam\\%s%s%s", (sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", "*.*");
    HANDLE hFile = api->fio->_FindFirstFile(file_mask, &wfd);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (wfd.cFileName[0] == L'.')
                    continue;

                std::string FileName = utf8::ConvertWideToUtf8(wfd.cFileName);
                sprintf(file_mask, "%s%s%s", (sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", FileName.c_str());
                DoZapSuperGeneratorInnerDecodeFiles(file_mask, mask);
            }
        } while (api->fio->_FindNextFile(hFile, &wfd));
        FindClose(hFile);
    }

    sprintf(file_mask, "resource\\foam\\%s%s%s", (sub_dir) ? sub_dir : "", (sub_dir) ? "\\" : "", mask);

    hFile = api->fio->_FindFirstFile(file_mask, &wfd);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string FileName = utf8::ConvertWideToUtf8(wfd.cFileName);
            sprintf(file_mask, "resource\\foam\\%s\\%s", (sub_dir) ? sub_dir : "", FileName.c_str());
            DoZapSuperGeneratorDecodeFile(file_mask);
        } while (api->fio->_FindNextFile(hFile, &wfd));
        FindClose(hFile);
    }
}

void ISLAND::DoZapSuperGenerator()
{
    DoZapSuperGeneratorInnerDecodeFiles(0, "*.tga");
}