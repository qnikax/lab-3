#include <iostream>
#include <locale>
#include <string>
#include "routeCipher.h"

using namespace std;

// ===================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =====================
int total_tests = 0;
int passed_tests = 0;

void assert_true(bool condition, const string& message) {
    total_tests++;
    if (condition) {
        passed_tests++;
        cout << "✓ " << message << endl;
    } else {
        cout << "✗ " << message << endl;
    }
}

void assert_exception(void (*func)(), const string& message) {
    total_tests++;
    try {
        func();
        cout << "✗ " << message << " (ожидалось исключение)" << endl;
    } catch (const route_cipher_error& e) {
        passed_tests++;
        cout << "✓ " << message << endl;
    } catch (...) {
        cout << "✗ " << message << " (неожиданное исключение)" << endl;
    }
}

void print_section(const string& section_name) {
    cout << "\n" << string(60, '=') << endl;
    cout << section_name << endl;
    cout << string(60, '=') << endl;
}

// ===================== ТЕСТЫ КОНСТРУКТОРА =====================
void test_constructor() {
    print_section("ТЕСТЫ КОНСТРУКТОРА ROUTECIPHER");
    
    // Корректные значения столбцов
    assert_true([]() {
        try { routeCipher cipher(1); return true; } catch (...) { return false; }
    }(), "Создание с 1 столбцом");
    
    assert_true([]() {
        try { routeCipher cipher(5); return true; } catch (...) { return false; }
    }(), "Создание с 5 столбцами");
    
    assert_true([]() {
        try { routeCipher cipher(10); return true; } catch (...) { return false; }
    }(), "Создание с 10 столбцами");
    
    // Некорректные значения
    assert_exception([]() {
        routeCipher cipher(0);
    }, "Нулевое количество столбцов");
    
    assert_exception([]() {
        routeCipher cipher(-5);
    }, "Отрицательное количество столбцов");
    
    assert_exception([]() {
        routeCipher cipher(101);
    }, "Слишком большое количество столбцов");
}

// ===================== ТЕСТЫ ШИФРОВАНИЯ =====================
void test_encrypt() {
    print_section("ТЕСТЫ ШИФРОВАНИЯ (ENCRYPT)");
    
    // Базовое шифрование
    assert_true([]() {
        routeCipher cipher(3);
        wstring result = cipher.encrypt(L"ПРИВЕТ");
        return !result.empty();
    }(), "Базовое шифрование");
    
    // Автоматическое приведение к верхнему регистру
    assert_true([]() {
        routeCipher cipher(4);
        wstring upper = cipher.encrypt(L"ТЕКСТ");
        wstring lower = cipher.encrypt(L"текст");
        return upper == lower;
    }(), "Автоматическое приведение к верхнему регистру");
    
    // Удаление пробелов
    assert_true([]() {
        routeCipher cipher(2);
        wstring clean = cipher.encrypt(L"ТЕКСТ");
        wstring with_spaces = cipher.encrypt(L"Т Е К С Т");
        return clean == with_spaces;
    }(), "Удаление пробелов при шифровании");
    
    // Удаление не-букв
    assert_true([]() {
        routeCipher cipher(3);
        wstring clean = cipher.encrypt(L"ПРИВЕТ");
        wstring dirty = cipher.encrypt(L"ПРИ123ВЕТ!@#");
        return clean == dirty;
    }(), "Удаление не-буквенных символов");
    
    // Исключения
    assert_exception([]() {
        routeCipher cipher(4);
        cipher.encrypt(L"");
    }, "Пустой текст для шифрования");
    
    assert_exception([]() {
        routeCipher cipher(3);
        cipher.encrypt(L"123!@#");
    }, "Текст без букв для шифрования");
    
    // Разное количество столбцов
    assert_true([]() {
        routeCipher cipher1(2);
        routeCipher cipher2(4);
        wstring text = L"ПРОГРАММИРОВАНИЕ";
        wstring enc1 = cipher1.encrypt(text);
        wstring enc2 = cipher2.encrypt(text);
        return enc1 != enc2; // Разные столбцы = разные результаты
    }(), "Разное количество столбцов дает разные результаты");
    
    // Проверка конкретного примера
    assert_true([]() {
        routeCipher cipher(3);
        wstring result = cipher.encrypt(L"ПРИВЕТМИР");
        // Проверяем, что результат имеет правильную длину
        return result.length() == 9; // "ПРИВЕТМИР" без пробелов = 9 символов
    }(), "Шифрование текста конкретной длины");
}

// ===================== ТЕСТЫ РАСШИФРОВАНИЯ =====================
void test_decrypt() {
    print_section("ТЕСТЫ РАСШИФРОВАНИЯ (DECRYPT)");
    
    // Полный цикл шифрование-расшифрование
    assert_true([]() {
        routeCipher cipher(4);
        wstring original = L"СЕКРЕТНОЕСООБЩЕНИЕ";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Полный цикл шифрование-расшифрование");
    
    // Разные тексты
    assert_true([]() {
        routeCipher cipher(3);
        
        wstring text1 = L"КОРОТКИЙ";
        wstring enc1 = cipher.encrypt(text1);
        wstring dec1 = cipher.decrypt(enc1);
        bool test1 = (text1 == dec1);
        
        wstring text2 = L"СРЕДНИЙТЕКСТ";
        wstring enc2 = cipher.encrypt(text2);
        wstring dec2 = cipher.decrypt(enc2);
        bool test2 = (text2 == dec2);
        
        wstring text3 = L"ОЧЕНЬДЛИННЫЙТЕКСТДЛЯПРОВЕРКИ";
        wstring enc3 = cipher.encrypt(text3);
        wstring dec3 = cipher.decrypt(enc3);
        bool test3 = (text3 == dec3);
        
        return test1 && test2 && test3;
    }(), "Расшифрование разных текстов");
    
    // Исключения
    assert_exception([]() {
        routeCipher cipher(5);
        cipher.decrypt(L"");
    }, "Пустой шифротекст");
    
    assert_exception([]() {
        routeCipher cipher(4);
        cipher.decrypt(L"строчныебуквы");
    }, "Шифротекст в нижнем регистре");
    
    assert_exception([]() {
        routeCipher cipher(3);
        cipher.decrypt(L"ШИФР123ТЕКСТ");
    }, "Шифротекст с цифрами");
    
    assert_exception([]() {
        routeCipher cipher(2);
        cipher.decrypt(L"ШИФР!ТЕКСТ");
    }, "Шифротекст со спецсимволами");
}

// ===================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ =====================
void test_edge_cases() {
    print_section("ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ");
    
    // Один столбец
    assert_true([]() {
        routeCipher cipher(1);
        wstring original = L"ТЕКСТ";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Шифрование с 1 столбцом");
    
    
    // Текст из одной буквы
    assert_true([]() {
        routeCipher cipher(1);
        wstring original = L"Я";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Текст из одной буквы");
    
    // Все буквы алфавита
    assert_true([]() {
        routeCipher cipher(6);
        wstring original = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Все буквы алфавита");
    
    // Прямоугольная таблица (текст кратен столбцам)
    assert_true([]() {
        routeCipher cipher(4);
        wstring original = L"ПРОГРАММИРОВАНИЕ"; // 16 символов, 4x4
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Прямоугольная таблица (текст кратен столбцам)");
    
    // Непрямоугольная таблица
    assert_true([]() {
        routeCipher cipher(5);
        wstring original = L"ПРОГРАММИРОВАНИЕПЕНЗА"; // 19 символов, 4x5 с остатком
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        return original == decrypted;
    }(), "Непрямоугольная таблица (текст не кратен столбцам)");
}

// ===================== ТЕСТЫ ИНТЕГРАЦИИ =====================
void test_integration() {
    print_section("ИНТЕГРАЦИОННЫЕ ТЕСТЫ");
    
    // Множественные операции
    assert_true([]() {
        routeCipher cipher(3);
        
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
    
    // Разное количество столбцов для одного текста
    assert_true([]() {
        wstring original = L"ОБЩЕЕСООБЩЕНИЕ";
        
        routeCipher cipher1(2);
        routeCipher cipher2(5);
        routeCipher cipher3(7);
        
        wstring enc1 = cipher1.encrypt(original);
        wstring enc2 = cipher2.encrypt(original);
        wstring enc3 = cipher3.encrypt(original);
        
        // Все должны успешно расшифровываться
        wstring dec1 = cipher1.decrypt(enc1);
        wstring dec2 = cipher2.decrypt(enc2);
        wstring dec3 = cipher3.decrypt(enc3);
        
        return (original == dec1) && (original == dec2) && (original == dec3);
    }(), "Разное количество столбцов для одного текста");
    
    // Проверка, что encrypt и decrypt - обратные операции
    assert_true([]() {
        routeCipher cipher(4);
        
        // Для нескольких случайных текстов
        wstring texts[] = {
            L"КОМПЬЮТЕР",
            L"ПРОГРАММА",
            L"АЛГОРИТМ",
            L"ШИФРОВАНИЕ",
            L"ДЕШИФРОВКА"
        };
        
        for (const auto& text : texts) {
            wstring encrypted = cipher.encrypt(text);
            wstring decrypted = cipher.decrypt(encrypted);
            if (text != decrypted) {
                return false;
            }
        }
        return true;
    }(), "Encrypt/decrypt - обратные операции для разных текстов");
}

// ===================== ГЛАВНАЯ ФУНКЦИЯ =====================
int main() {
    // Настройка локали
    locale::global(locale("ru_RU.UTF-8"));
    wcout.imbue(locale());
    
    cout << "\n" << string(70, '=') << endl;
    cout << "МОДУЛЬНОЕ ТЕСТИРОВАНИЕ ШИФРА МАРШРУТНОЙ ПЕРЕСТАНОВКИ" << endl;
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
