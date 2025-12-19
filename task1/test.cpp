#include <iostream>
#include <locale>
#include <cctype>
#include <codecvt>
#include <string>
#include "modAlphaCipher.h"

using namespace std;

// ===================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =====================
bool test_passed = true;
int total_tests = 0;
int passed_tests = 0;

void assert_true(bool condition, const string& message) {
    total_tests++;
    if (condition) {
        passed_tests++;
        cout << "✓ " << message << endl;
    } else {
        test_passed = false;
        cout << "✗ " << message << endl;
    }
}

void assert_exception(void (*func)(), const string& message) {
    total_tests++;
    try {
        func();
        test_passed = false;
        cout << "✗ " << message << " (ожидалось исключение)" << endl;
    } catch (const cipher_error& e) {
        passed_tests++;
        cout << "✓ " << message << endl;
    } catch (...) {
        test_passed = false;
        cout << "✗ " << message << " (неожиданное исключение)" << endl;
    }
}

void print_section(const string& section_name) {
    cout << "\n" << string(60, '=') << endl;
    cout << section_name << endl;
    cout << string(60, '=') << endl;
}

// ===================== ТЕСТЫ ДЛЯ КОНСТРУКТОРА =====================
void test_constructor() {
    print_section("ТЕСТЫ КОНСТРУКТОРА");
    
    // Корректные ключи
    assert_true([]() {
        try { modAlphaCipher cipher(L"КЛЮЧ"); return true; } 
        catch (...) { return false; }
    }(), "Создание с корректным ключом");
    
    assert_true([]() {
        try { modAlphaCipher cipher(L"длинныйключсразнымисимволами"); return true; } 
        catch (...) { return false; }
    }(), "Создание с длинным ключом");
    
    // Некорректные ключи
    assert_exception([]() {
        modAlphaCipher cipher(L"");
    }, "Пустой ключ");
    
    assert_exception([]() {
        modAlphaCipher cipher(L"ключ123");
    }, "Ключ с цифрами");
    
    assert_exception([]() {
        modAlphaCipher cipher(L"ключ!@#");
    }, "Ключ со спецсимволами");
    
    assert_exception([]() {
        modAlphaCipher cipher(L"ключ с пробелами");
    }, "Ключ с пробелами");
}

// ===================== ТЕСТЫ ДЛЯ ШИФРОВАНИЯ =====================
void test_encrypt() {
    print_section("ТЕСТЫ ШИФРОВАНИЯ (ENCRYPT)");
    
    // Базовое шифрование
    assert_true([]() {
        modAlphaCipher cipher(L"А");
        wstring result = cipher.encrypt(L"ПРИВЕТ");
        return !result.empty();
    }(), "Базовое шифрование");
    
    
    
    // Удаление не-букв
    assert_true([]() {
        modAlphaCipher cipher(L"В");
        wstring clean = cipher.encrypt(L"ПРИВЕТ");
        wstring dirty = cipher.encrypt(L"ПРИ 123 ВЕТ !@#");
        return clean == dirty;
    }(), "Удаление не-буквенных символов");
    
    // Исключения
    assert_exception([]() {
        modAlphaCipher cipher(L"Г");
        cipher.encrypt(L"");
    }, "Пустой текст для шифрования");
    
    assert_exception([]() {
        modAlphaCipher cipher(L"Д");
        cipher.encrypt(L"123!@#");
    }, "Текст без букв для шифрования");
    
    // Длинный текст
    assert_true([]() {
        modAlphaCipher cipher(L"ШИФР");
        wstring original = L"ЭТООЧЕНЬДЛИННЫЙТЕКСТДЛЯПРОВЕРКИРАБОТЫШИФРА";
        wstring encrypted = cipher.encrypt(original);
        return !encrypted.empty() && encrypted.length() == original.length();
    }(), "Шифрование длинного текста");
}

// ===================== ТЕСТЫ ДЛЯ РАСШИФРОВАНИЯ =====================
void test_decrypt() {
    print_section("ТЕСТЫ РАСШИФРОВАНИЯ (DECRYPT)");
    
    // Полный цикл шифрование-расшифрование
    assert_true([]() {
        modAlphaCipher cipher(L"СЕКРЕТ");
        wstring original = L"СЕКРЕТНОЕСООБЩЕНИЕ";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Полный цикл шифрование-расшифрование");
    
    // Разные ключи
    assert_true([]() {
        modAlphaCipher cipher1(L"КЛЮЧИК");
        modAlphaCipher cipher2(L"КЛЮЧИК");
        wstring original = L"ТЕСТОВОЕСООБЩЕНИЕ";
        wstring encrypted = cipher1.encrypt(original);
        wstring decrypted = cipher2.decrypt(encrypted);
        return original == decrypted;
    }(), "Расшифрование другим экземпляром с тем же ключом");
    
    // Исключения
    assert_exception([]() {
        modAlphaCipher cipher(L"Е");
        cipher.decrypt(L"");
    }, "Пустой шифротекст");
    
    assert_exception([]() {
        modAlphaCipher cipher(L"Ё");
        cipher.decrypt(L"строчныебуквы");
    }, "Шифротекст в нижнем регистре");
    
    assert_exception([]() {
        modAlphaCipher cipher(L"Ж");
        cipher.decrypt(L"ШИФР123ТЕКСТ");
    }, "Шифротекст с цифрами");
    
    assert_exception([]() {
        modAlphaCipher cipher(L"З");
        cipher.decrypt(L"ШИФР!ТЕКСТ");
    }, "Шифротекст со спецсимволами");
}

// ===================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ =====================
void test_edge_cases() {
    print_section("ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ");
    
    // Ключ длиннее текста
    assert_true([]() {
        modAlphaCipher cipher(L"ОЧЕНЬДЛИННЫЙКЛЮЧКОТОРЫЙДЛИННЕЕТЕКСТА");
        wstring original = L"КОРОТКИЙ";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Ключ длиннее текста");
    
    // Текст длиннее ключа
    assert_true([]() {
        modAlphaCipher cipher(L"КОР");
        wstring original = L"ОЧЕНЬДЛИННЫЙТЕКСТКОТОРЫЙНАМНОГОДЛИННЕЕКЛЮЧА";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Текст длиннее ключа");
    
    // Один символ
    assert_true([]() {
        modAlphaCipher cipher(L"Я");
        wstring original = L"А";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Текст из одного символа");
    
    // Все буквы алфавита
    assert_true([]() {
        modAlphaCipher cipher(L"АЛФАВИТ");
        wstring original = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Все буквы алфавита");
}

// ===================== ТЕСТЫ ИНТЕГРАЦИИ =====================
void test_integration() {
    print_section("ИНТЕГРАЦИОННЫЕ ТЕСТЫ");
    
    // Множественные операции
    assert_true([]() {
        modAlphaCipher cipher(L"МАСТЕРКЛЮЧ");
        
        // Тест 1
        wstring text1 = L"ПЕРВОЕСООБЩЕНИЕ";
        wstring enc1 = cipher.encrypt(text1);
        wstring dec1 = cipher.decrypt(enc1);
        bool test1 = (text1 == dec1);
        
        // Тест 2
        wstring text2 = L"ВТОРОЕСООБЩЕНИЕ";
        wstring enc2 = cipher.encrypt(text2);
        wstring dec2 = cipher.decrypt(enc2);
        bool test2 = (text2 == dec2);
        
        // Тест 3
        wstring text3 = L"ТРЕТЬЕСООБЩЕНИЕ";
        wstring enc3 = cipher.encrypt(text3);
        wstring dec3 = cipher.decrypt(enc3);
        bool test3 = (text3 == dec3);
        
        return test1 && test2 && test3;
    }(), "Множественные шифрования одним шифром");
    
    // Разные шифры
    assert_true([]() {
        wstring original = L"ОБЩЕЕСООБЩЕНИЕ";
        
        modAlphaCipher cipher1(L"ПЕРВЫЙКЛЮЧ");
        modAlphaCipher cipher2(L"ВТОРОЙКЛЮЧ");
        
        wstring enc1 = cipher1.encrypt(original);
        wstring enc2 = cipher2.encrypt(original);
        
        return enc1 != enc2; // Разные ключи должны давать разные результаты
    }(), "Разные ключи дают разные результаты");
}

// ===================== ГЛАВНАЯ ФУНКЦИЯ =====================
int main() {
    // Настройка локали
    locale::global(locale("ru_RU.UTF-8"));
    wcout.imbue(locale());
    
    cout << "\n" << string(70, '=') << endl;
    cout << "МОДУЛЬНОЕ ТЕСТИРОВАНИЕ ШИФРА ГРОНСФЕЛЬДА" << endl;
    cout << string(70, '=') << endl;
    
    // Запуск всех тестов
    test_constructor();
    test_encrypt();
    test_decrypt();
    test_edge_cases();
    test_integration();
    
    // Итоги
    cout << "\n" << string(70, '=') << endl;
    cout << "ИТОГИ ТЕСТИРОВАНИЯ" << endl;
    cout << string(70, '=') << endl;
    
    cout << "Всего тестов: " << total_tests << endl;
    cout << "Пройдено: " << passed_tests << endl;
    cout << "Не пройдено: " << (total_tests - passed_tests) << endl;
    
    if (passed_tests == total_tests) {
        cout << "\n✓ ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ!" << endl;
        return 0;
    } else {
        cout << "\n✗ ТЕСТИРОВАНИЕ НЕ УСПЕШНО" << endl;
        return 1;
    }
}
