#include "routeCipher.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <cwctype>

routeCipher::routeCipher(int cols)
{
    validateColumns(cols);
    columns = cols;
}

// Валидация количества столбцов
void routeCipher::validateColumns(int cols)
{
    if (cols <= 0) {
        throw route_cipher_error("Number of columns must be positive");
    }
    if (cols > 100) { // Реалистичное ограничение
        throw route_cipher_error("Number of columns is too large");
    }
}

// Валидация открытого текста
std::wstring routeCipher::getValidOpenText(const std::wstring& s)
{
    if (s.empty()) {
        throw route_cipher_error("Empty open text");
    }
    
    std::wstring result;
    std::locale loc("ru_RU.UTF-8");
    
    for (wchar_t c : s) {
        if (isalpha(c, loc) || c == L' ') {
            if (isalpha(c, loc)) {
                c = toupper(c, loc);
            }
            result += c;
        }
        // Игнорируем другие символы (цифры, знаки препинания)
    }
    
    // Удаляем пробелы для шифрования
    std::wstring textWithoutSpaces;
    for (wchar_t c : result) {
        if (c != L' ') {
            textWithoutSpaces += c;
        }
    }
    
    if (textWithoutSpaces.empty()) {
        throw route_cipher_error("Open text contains no valid letters");
    }
    
    return textWithoutSpaces;
}

// Валидация зашифрованного текста
std::wstring routeCipher::getValidCipherText(const std::wstring& s)
{
    if (s.empty()) {
        throw route_cipher_error("Empty cipher text");
    }
    
    std::locale loc("ru_RU.UTF-8");
    for (wchar_t c : s) {
        if (!isalpha(c, loc)) {
            throw route_cipher_error("Cipher text must contain only letters");
        }
        if (!isupper(c, loc)) {
            throw route_cipher_error("Cipher text must be in uppercase");
        }
    }
    
    return s;
}

std::wstring routeCipher::prepareText(const std::wstring& text)
{
    std::wstring result;
    std::locale loc("ru_RU.UTF-8");
    
    for (wchar_t c : text) {
        if (c != L' ') {
            c = toupper(c, loc);
            result += c;
        }
    }
    return result;
}

std::vector<std::vector<wchar_t>> routeCipher::createTable(const std::wstring& text, int cols)
{
    int length = text.length();
    int rows = (length + cols - 1) / cols;
    std::vector<std::vector<wchar_t>> table(rows, std::vector<wchar_t>(cols, L' '));

    int index = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (index < length) {
                table[i][j] = text[index++];
            }
        }
    }
    return table;
}

std::wstring routeCipher::readEncrypted(const std::vector<std::vector<wchar_t>>& table, int cols)
{
    int rows = table.size();
    std::wstring result;
    for (int j = cols - 1; j >= 0; j--) {
        for (int i = 0; i < rows; i++) {
            if (table[i][j] != L' ') {
                result += table[i][j];
            }
        }
    }
    return result;
}

std::wstring routeCipher::readDecrypted(const std::vector<std::vector<wchar_t>>& table, int cols)
{
    int rows = table.size();
    std::wstring result;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (table[i][j] != L' ') {
                result += table[i][j];
            }
        }
    }
    return result;
}

std::wstring routeCipher::encrypt(const std::wstring& text)
{
    std::wstring prepared = getValidOpenText(text);
    auto table = createTable(prepared, columns);
    return readEncrypted(table, columns);
}

std::wstring routeCipher::decrypt(const std::wstring& text)
{
    std::wstring prepared = getValidCipherText(text);
    int length = prepared.length();
    
    if (length == 0) {
        throw route_cipher_error("Cipher text is empty after validation");
    }
    
    int rows = (length + columns - 1) / columns;

    int extras = length % columns;
    std::vector<int> heights(columns, rows);
    if (extras != 0) {
        for (int j = 0; j < columns; ++j) {
            heights[j] = (j < extras) ? rows : (rows - 1);
        }
    }

    std::vector<std::vector<wchar_t>> table(rows, std::vector<wchar_t>(columns, L' '));
    int index = 0;

    for (int j = columns - 1; j >= 0; j--) {
        int h = heights[j];
        for (int i = 0; i < h; i++) {
            if (index < length) {
                table[i][j] = prepared[index++];
            }
        }
    }
    return readDecrypted(table, columns);
}