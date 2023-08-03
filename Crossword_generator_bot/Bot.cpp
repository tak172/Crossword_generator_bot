#include <stdio.h>
#include "Generator.h"
#include <tgbot/tgbot.h>
#include "sqlite3.h"

long long users_count = -1;

struct simbol_code {

    int part1;
    int part2;
    char simbol;

    simbol_code(char simbol) : part1(0), part2(0), simbol(simbol) { }
    simbol_code(int p1, int p2, char simbol) : part1(p1), part2(p2), simbol(simbol) { }

};

void IncorrectMessages(TgBot::Bot& bot, TgBot::Message::Ptr& message, std::string&& mes, bool language_flag) {

    if (language_flag) bot.getApi().sendMessage(message->chat->id, mes +
        " Use /instruction for information on how to use the bot. \xF0\x9F\x94\xA7");
    else bot.getApi().sendMessage(message->chat->id, mes +
        " Используйте команду /instruction, для получения информации о боте. \xF0\x9F\x94\xA7");

}

void Instruction(TgBot::Bot& bot, TgBot::Message::Ptr& message, bool language_flag) {

    if (language_flag) bot.getApi().sendMessage(message->chat->id,
        "\xF0\x9F\x94\xB8 To start, enter the /start command and select the language.\n"
        "\xF0\x9F\x94\xB8 To get instructions for use, enter /instruction. \xF0\x9F\x94\xA7\n"
        "\xF0\x9F\x94\xB8 To start generating the crossword, type /generate.\nThen you need to enter the number of "
        "words that the crossword will consist of. (2 - 20)\n"
        "Then enter the words, one at a time in the message.\nThe word length can be no more than 20 characters.\n"
        "A word can consist only of the letters of the alphabet of the language you have chosen.\n"
        "After making sure that the entered words are correct, click \"YES\" and expect the result.\n"
        "(up to 5 minutes) \xE2\x8F\xB0\n"
        "\xE2\xAD\x90 Have a good use! \xE2\xAD\x90");

    else bot.getApi().sendMessage(message->chat->id,
        "\xF0\x9F\x94\xB8 Используйте команду /start, для начала работы и выбора языка.\n"
        "\xF0\x9F\x94\xB8 Используйте команду /instruction, для получения информации о боте. \xF0\x9F\x94\xA7\n"
        "\xF0\x9F\x94\xB8 Для того чтобы приступить к генерации кроссворда, используйте команду /generate.\n"
        "Затем введите число слов, из которых будет состоять кроссворд. (2 - 20)\n"
        "Затем введите слова, по одному в сообщении.\nДлина слова не более 20 символов.\n"
        "Слово может состоять только из букв алфавита выбранного вами языка.\n"
        "После убедитесь в правильности введенной информации и нажмите \"YES\". Ожидайте результат."
        "\n(занимает до 5 минут) \xE2\x8F\xB0\n"
        "\xE2\xAD\x90 Приятного использования! \xE2\xAD\x90");

}

void MakeTwoButtonsKeyboard(TgBot::InlineKeyboardMarkup::Ptr& keyboard, const std::string& b1, const std::string& b2) {

    std::vector<TgBot::InlineKeyboardButton::Ptr> buttons{ std::make_shared<TgBot::InlineKeyboardButton>(),
        std::make_shared<TgBot::InlineKeyboardButton>() };
    buttons[0]->text = b1;
    buttons[0]->callbackData = b1;
    buttons[1]->text = b2;
    buttons[1]->callbackData = b2;
    keyboard->inlineKeyboard.push_back(buttons);

}

void Decoding(std::vector<std::string>& words, std::vector<simbol_code>& simbols_code, bool language_flag) {

    if (language_flag) {

        for (auto& word : words) {
            for (auto& sim : word) {
                sim = std::toupper(sim);
            }
        }

        return;
    }

    std::vector<std::string> new_words(words.size());

    for (size_t word = 0; word < words.size(); ++word) {
        for (size_t sim = 0; sim < words[word].size(); sim += 2) {
            for (const auto& code : simbols_code) {

                if (static_cast<int>(words[word][sim]) == code.part1 &&
                    static_cast<int>(words[word][sim + 1]) == code.part2) {
                    new_words[word] += code.simbol;
                    break;
                }

            }
        }
    }

    words = std::move(new_words);

}

bool CheckWord(TgBot::Message::Ptr& message, bool language_flag) {

    std::string message_text = message->text;

    if (language_flag) {
        if (message_text.size() > 20 || !message_text.size() || message_text.find(' ') != std::string::npos)
            return false;

        for (auto& sim : message_text) {
            if (!(std::toupper(sim) >= 'A' && std::toupper(sim) <= 'Z'))
                return false;
        }
    }
    else {

        if (message_text.size() > 40 || !message_text.size() || message_text.find(' ') != std::string::npos)
            return false;

        for (size_t sim = 0; sim < message_text.size(); sim += 2) {

            if (static_cast<int>(message_text[sim]) == -48 && ((static_cast<int>(message_text[sim + 1]) >= -112
                && static_cast<int>(message_text[sim + 1]) <= -65) || static_cast<int>(message_text[sim + 1]) == -127))
                continue;
            else if (static_cast<int>(message_text[sim]) == -47 && ((static_cast<int>(message_text[sim + 1]) >= -128
                && static_cast<int>(message_text[sim + 1]) <= -113) || static_cast<int>(message_text[sim + 1]) == -111))
                continue;
            else
                return false;

        }
    }

    return true;

}

void BadEnter(std::vector<std::string>& words, int& word_num, bool& word_input_flag) {

    words.clear();
    word_num = 0;
    word_input_flag = false;

}

void GenerateCrossword(std::vector<std::string>& words, TgBot::Bot& bot, TgBot::Message::Ptr& message, std::vector<simbol_code>& simbols_code, bool language_flag) {

    bot.getApi().sendVideo(message->chat->id, TgBot::InputFile::fromFile("giphy.gif.mp4", "video/mp4"));

    Generator g(words);
    g.Generate(1, Generator::Direction::VERTICALLY);

    if (language_flag)
        bot.getApi().sendMessage(message->chat->id, "Generation completed.");
    else
        bot.getApi().sendMessage(message->chat->id, "Генерация завершилась.");

    std::string crossword = g.GetCrossword(language_flag);

    if (!language_flag) {

        std::string temp;
        bool simbol_flag;

        for (auto& sim : crossword) {
            simbol_flag = false;
            for (auto& code : simbols_code) {
                if (sim == code.simbol) {
                    simbol_flag = true;
                    temp += static_cast<char>(code.part1);
                    temp += static_cast<char>(code.part2);
                    break;
                }
            }
            if (!simbol_flag)
                temp += sim;
        }

        crossword = std::move(temp);

    }

    std::ofstream file("crossword.rtf");
    file << crossword;
    file.close();

    file.open("crossword.html");
    file << crossword;
    file.close();

    bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile("crossword.rtf", "file/rtf"));
    bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile("crossword.html", "file/html"));

}

int callback(void* data, int argc, char** argv, char** azColName) {
    users_count = std::stoi(argv[0]);
    return 0;
}

int main(int argc, char* argv[]) {

    std::ifstream file("token.txt");
    std::string token;
    getline(file, token);
    file.close();

    TgBot::Bot bot(token);

    TgBot::InlineKeyboardMarkup::Ptr keyboard_lang(new TgBot::InlineKeyboardMarkup);
    MakeTwoButtonsKeyboard(keyboard_lang, "RU \xF0\x9F\x87\xB7\xF0\x9F\x87\xBA", "EN \xF0\x9F\x87\xAC\xF0\x9F\x87\xA7");

    TgBot::InlineKeyboardMarkup::Ptr keyboard_gen(new TgBot::InlineKeyboardMarkup);
    MakeTwoButtonsKeyboard(keyboard_gen, "YES \xE2\x9C\x85", "NO \xE2\x9D\x8C");

    bool language_flag = true;
    bool number_input_flag = false;
    bool word_input_flag = false;
    bool block_flag = false;
    int word_num = 0;

    std::string entered_words;
    std::vector<std::string> words;
    std::vector<simbol_code> simbols_code{
        '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307', '\310', '\311', '\312',
        '\313', '\314', '\315', '\316', '\317', '\320', '\321', '\322', '\323', '\324', '\325',
        '\326', '\327', '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337', '\300',
        '\301', '\302', '\303', '\304', '\305', '\306', '\307', '\310', '\311', '\312', '\313',
        '\314', '\315', '\316', '\317', '\320', '\321', '\322', '\323', '\324', '\325', '\326',
        '\327', '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337'
    };

    for (int i = 0; i < 65; ++i) {

        if (i < 48) {
            simbols_code[i].part1 = -48;
            simbols_code[i].part2 = -112 + i;
        }
        else if (i < 64) {
            simbols_code[i].part1 = -47;
            simbols_code[i].part2 = -176 + i;
        }
        else {
            simbols_code.emplace_back(-48, -127, '\250');
            simbols_code.emplace_back(-47, -111, '\250');
        }

    }

    sqlite3* db;

    bot.getEvents().onUnknownCommand([&bot, &number_input_flag, &word_input_flag, &words,
        &word_num, &block_flag, &language_flag](TgBot::Message::Ptr message) {

            if (block_flag)
            return;

    BadEnter(words, word_num, word_input_flag);
    number_input_flag = false;

    if (language_flag)
        IncorrectMessages(bot, message, "Unknown Command.", language_flag);
    else
        IncorrectMessages(bot, message, "Неизвестная команда.", language_flag);

        });

    bot.getEvents().onCommand("instruction", [&bot, &number_input_flag, &word_input_flag, &words,
        &word_num, &block_flag, &language_flag](TgBot::Message::Ptr message) {

        if (block_flag)
            return;

        BadEnter(words, word_num, word_input_flag);
        number_input_flag = false;
        Instruction(bot, message, language_flag);

    });

    bot.getEvents().onCommand("start", [&bot, &number_input_flag, &keyboard_lang, &word_input_flag,
        &words, &word_num, &block_flag, &language_flag, &db](TgBot::Message::Ptr message) {

        if (block_flag)
            return;

        const std::string QUERY(
            "CREATE TABLE IF NOT EXISTS USERS("
            "USER_ID	INTEGER		NOT NULL,"
            "UNIQUE(USER_ID)"
            ");"
            "INSERT OR IGNORE INTO USERS(USER_ID) VALUES(" + std::to_string(message->from->id) + ");"
            "SELECT COUNT(*) from USERS; ");

        sqlite3_open("USERS.db", &db);
        sqlite3_exec(db, QUERY.c_str(), callback, 0, nullptr);
        sqlite3_close(db);
    
        BadEnter(words, word_num, word_input_flag);
        number_input_flag = false;
    
        if (language_flag)
            bot.getApi().sendMessage(message->chat->id, "Hi \xF0\x9F\x91\x8B " + message->chat->firstName + "! Choose the "
                "language in which it is convenient for you to communicate with me.", false, 0, keyboard_lang);
        else
            bot.getApi().sendMessage(message->chat->id, "Привет \xF0\x9F\x91\x8B " + message->chat->firstName + "! Выберите "
                "язык на котором вам удобнее со мной общаться.", false, 0, keyboard_lang);

    });

    bot.getEvents().onCallbackQuery([&bot, &number_input_flag, &language_flag, &word_input_flag,
        &words, &word_num, &block_flag, &simbols_code](TgBot::CallbackQuery::Ptr query) {

        if (block_flag)
            return;

        number_input_flag = false;
    
        if (query->data == "NO \xE2\x9D\x8C" || query->data == "YES \xE2\x9C\x85") {
            if (word_input_flag) {
                if (language_flag)
                    bot.getApi().sendMessage(query->message->chat->id, "You didn't enter all the words. \xE2\x9C\x89");
                else
                    bot.getApi().sendMessage(query->message->chat->id, "Вы ввели не все слова. \xE2\x9C\x89");
            }
            else if (query->data == "NO \xE2\x9D\x8C") {
                BadEnter(words, word_num, word_input_flag);
                if (language_flag)
                    IncorrectMessages(bot, query->message, "Words dropped.", language_flag);
                else
                    IncorrectMessages(bot, query->message, "Слова сброшены.", language_flag);
            }
            else if (words.size()) {
                if (language_flag)
                    bot.getApi().sendMessage(query->message->chat->id,
                        "Words accepted! Expect a crossword puzzle! This may take up to 5 minutes. \xE2\x8F\xB0");
                else
                    bot.getApi().sendMessage(query->message->chat->id,
                        "Слова приняты! Начинаю состовлять кроссворд! Это может занять до 5 минут. \xE2\x8F\xB0");
    
                block_flag = true;
                Decoding(words, simbols_code, language_flag);
                GenerateCrossword(words, bot, query->message, simbols_code, language_flag);
                words.clear();
                block_flag = false;
            }
            return;
        }

        BadEnter(words, word_num, word_input_flag);
    
        if (query->data == "RU \xF0\x9F\x87\xB7\xF0\x9F\x87\xBA") {
            bot.getApi().sendMessage(query->message->chat->id, "Выбран русский \xF0\x9F\x87\xB7\xF0\x9F\x87\xBA");
            language_flag = false;
            Instruction(bot, query->message, language_flag);
        }
        else if (query->data == "EN \xF0\x9F\x87\xAC\xF0\x9F\x87\xA7") {
            bot.getApi().sendMessage(query->message->chat->id, "Selected English \xF0\x9F\x87\xAC\xF0\x9F\x87\xA7");
            language_flag = true;
            Instruction(bot, query->message, language_flag);
        }

    });

    bot.getEvents().onCommand("generate", [&bot, &number_input_flag, &words, &word_num,
        &word_input_flag, &block_flag, &language_flag](TgBot::Message::Ptr message) {

        if (block_flag)
            return;

        BadEnter(words, word_num, word_input_flag);

        if (language_flag)
            bot.getApi().sendMessage(message->chat->id, "Enter number of words (2 - 20):");
        else
            bot.getApi().sendMessage(message->chat->id, "Введите число слов (2 - 20):");
    
        number_input_flag = true;

    });

    bot.getEvents().onNonCommandMessage([&bot, &number_input_flag, &word_num, &word_input_flag,
        &words, &keyboard_gen, &entered_words, &block_flag, &language_flag](TgBot::Message::Ptr message) {

        if (block_flag)
            return;

        if (word_input_flag) {
            if (!CheckWord(message, language_flag)) {
                if (language_flag)
                    bot.getApi().sendMessage(message->chat->id,
                        "\xF0\x9F\x9A\xAB The word does not match the requirements, try to enter a new one:");
                else
                    bot.getApi().sendMessage(message->chat->id,
                        "\xF0\x9F\x9A\xAB Слово не соответствует требованиям, попробуйте ввести новое:");
                return;
            }

            words.push_back(message->text);
            --word_num;

            if (!word_num) {
                word_input_flag = false;
    
                for (const auto& word : words)
                    entered_words += word + '\n';
    
                if (language_flag)
                    bot.getApi().sendMessage(message->chat->id, entered_words +
                        "Are all words entered correctly?", false, 0, keyboard_gen);
                else
                    bot.getApi().sendMessage(message->chat->id, entered_words +
                        "Все слова введены правильно?", false, 0, keyboard_gen);
    
                entered_words.clear();
            }
            return;
        }
        try {
            if (number_input_flag) {
                word_num = std::stoi(message->text);
    
                if (word_num < 1 || word_num > 20)
                    throw std::invalid_argument(message->text);
    
                if (language_flag)
                    bot.getApi().sendMessage(message->chat->id, "\xE2\x9A\xA1 Enter " + message->text +
                        " words from which I will generate a crossword puzzle for you:");
                else
                    bot.getApi().sendMessage(message->chat->id, "\xE2\x9A\xA1 Введите " + message->text +
                        " слов, из которых я сгенерирую кроссворд для вас:");
    
                number_input_flag = false;
                word_input_flag = true;
                return;
            }
            else
                throw std::invalid_argument(message->text);
        }
        catch (std::invalid_argument&) {
            word_num = 0;
            number_input_flag = false;
    
            if (language_flag)
                IncorrectMessages(bot, message, "Incorrest message.", language_flag);
            else
                IncorrectMessages(bot, message, "Неверное сообщение.", language_flag);
            return;
        }

    });

    bot.getEvents().onCommand("users_count", [&bot, &block_flag](TgBot::Message::Ptr message) {

        if (block_flag)
            return;

        if (users_count == -1)
            return;

        bot.getApi().sendMessage(message->chat->id, std::to_string(users_count));

    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            try {
                printf("Long poll started\n");
                longPoll.start();
            }
            catch (TgBot::TgException& e) {
                std::string one(e.what()), two("Forbidden: bot was blocked by the user");
                if (one == two) {
                    for (auto& up : bot.getApi().getUpdates())
                        up = nullptr;
                }
                else throw;
            }
        }
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
