#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <locale>

class route_cipher_error : public std::invalid_argument {
public:
    explicit route_cipher_error(const std::string& what_arg) :
        std::invalid_argument(what_arg) {}
    explicit route_cipher_error(const char* what_arg) :
        std::invalid_argument(what_arg) {}
};

class routeCipher
{
private:
    int columns;

    std::wstring prepareText(const std::wstring& text);
    std::vector<std::vector<wchar_t>> createTable(const std::wstring& text, int cols);
    std::wstring readEncrypted(const std::vector<std::vector<wchar_t>>& table, int cols);
    std::wstring readDecrypted(const std::vector<std::vector<wchar_t>>& table, int cols);
    
    // Методы валидации
    void validateColumns(int cols);
    std::wstring getValidOpenText(const std::wstring& s);
    std::wstring getValidCipherText(const std::wstring& s);

public:
    routeCipher() = delete;
    routeCipher(int cols);

    std::wstring encrypt(const std::wstring& text);
    std::wstring decrypt(const std::wstring& text);
};