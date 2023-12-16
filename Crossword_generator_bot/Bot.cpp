#include <stdio.h>
#include "Generator.h"
#include <tgbot/tgbot.h>
#include <sqlite3.h>

long long users_count = -1;

wchar_t Rus_upper( wchar_t sim )
{
    if ( sim >= L'а' && sim <= L'я' )
        return (sim - 32);
    else if ( sim == L'ё' )
        return L'Ё';
    else if ( sim == L'й' )
        return L'Й';
    return sim;
}

void IncorrectMessages( TgBot::Bot & bot, TgBot::Message::Ptr & message, const std::string & mes, bool language_flag )
{
    std::string message_text;
    if (language_flag)
        message_text = u8" Use /instruction for information on how to use the bot. 🔧";
    else
        message_text = u8" Используйте команду /instruction, для получения информации о боте. 🔧";

    bot.getApi().sendMessage( message->chat->id, mes + message_text );
}

void Instruction( TgBot::Bot & bot, TgBot::Message::Ptr & message, bool language_flag ) 
{
    std::string instruction_text;
    if ( language_flag )
        instruction_text = u8"🔸 To start, enter the /start command and select the language.\n"
        u8"🔸 To get instructions for use, enter /instruction. 🔧\n"
        u8"🔸 To start generating the crossword, type /generate.\n"
        u8"Then you need to enter the number of words that the crossword will consist of. (2 - 20)\n"
        u8"Then enter the words, one at a time in the message.\nThe word length can be no more than 20 characters.\n"
        u8"A word can consist only of the letters of the alphabet of the language you have chosen.\n"
        u8"After making sure that the entered words are correct, click \"YES\" and expect the result.\n"
        u8"(up to 5 minutes) ⏰\n"
        u8"⭐ Have a good use! ⭐";
    else 
        instruction_text = u8"🔸 Используйте команду /start, для начала работы и выбора языка.\n"
        u8"🔸 Используйте команду /instruction, для получения информации о боте. 🔧\n"
        u8"🔸 Для того чтобы приступить к генерации кроссворда, используйте команду /generate.\n"
        u8"Затем введите число слов, из которых будет состоять кроссворд. (2 - 20)\n"
        u8"Затем введите слова, по одному в сообщении.\nДлина слова не более 20 символов.\n"
        u8"Слово может состоять только из букв алфавита выбранного вами языка.\n"
        u8"После убедитесь в правильности введенной информации и нажмите \"YES\". Ожидайте результат."
        u8"\n(занимает до 5 минут) ⏰\n"
        u8"⭐ Приятного использования! ⭐";

    bot.getApi().sendMessage( message->chat->id, instruction_text );
}

void MakeTwoButtonsKeyboard( TgBot::InlineKeyboardMarkup::Ptr & keyboard, const std::string & b1, const std::string & b2 ) 
{
    std::vector<TgBot::InlineKeyboardButton::Ptr> buttons{ std::make_shared<TgBot::InlineKeyboardButton>(), std::make_shared<TgBot::InlineKeyboardButton>() };
    buttons[0]->text = b1;
    buttons[0]->callbackData = b1;
    buttons[1]->text = b2;
    buttons[1]->callbackData = b2;
    keyboard->inlineKeyboard.push_back( buttons );
}

bool CheckWord( TgBot::Message::Ptr& message, bool language_flag )
{
    std::string message_text = message->text;
    if ( message_text.size() > 20 || message_text.size() < 2 || message_text.find(u8' ') != std::string::npos )
        return false;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt_utf8;

    if ( language_flag ) 
    {
        for ( auto& sim : message_text ) 
        {
            if ( !( std::toupper( sim ) >= u8'A' && std::toupper( sim ) <= u8'Z' ) )
                return false;
        }
    }
    else 
    {
        for ( auto & sim : cvt_utf8.from_bytes( message_text ) )
        {
            if ( !( ( Rus_upper( sim ) >= L'А' && Rus_upper( sim ) <= L'Я' ) || Rus_upper(sim) == L'Й' || Rus_upper(sim) == L'Ё' ) )
                return false;
        }
    }
    return true;
}

void BadEnter(std::vector<std::string>& words, int& word_num, bool& word_input_flag) 
{
    words.clear();
    word_num = 0;
    word_input_flag = false;
}

void GenerateCrossword( std::vector<std::string> & words, TgBot::Bot & bot, TgBot::Message::Ptr & message, bool language_flag ) 
{
    bot.getApi().sendVideo( message->chat->id, TgBot::InputFile::fromFile( "giphy.gif.mp4", "video/mp4" ) );

    Generator g;
    g.Generate( words );

    if ( language_flag )
        bot.getApi().sendMessage( message->chat->id, u8"Generation completed." );
    else
        bot.getApi().sendMessage( message->chat->id, u8"Генерация завершилась." );

    std::string crossword = g.GetCrossword( language_flag );

    std::ofstream file( "crossword.rtf" );
    file << crossword;
    file.close();

    file.open( "crossword.html" );
    file << crossword;
    file.close();

    bot.getApi().sendDocument( message->chat->id, TgBot::InputFile::fromFile( "crossword.rtf", "file/rtf" ) );
    bot.getApi().sendDocument( message->chat->id, TgBot::InputFile::fromFile( "crossword.html", "file/html") );
}

int callback( void * data, int argc, char** argv, char** azColName ) 
{
    users_count = std::stoi( argv[0] );
    return 0;
}

int main( int argc, char* argv[] )
{
    std::ifstream file( "token.txt" );
    std::string token;
    getline(file, token);
    file.close();

    TgBot::Bot bot(token);
    TgBot::InlineKeyboardMarkup::Ptr keyboard_lang(new TgBot::InlineKeyboardMarkup);
    MakeTwoButtonsKeyboard( keyboard_lang, u8"RU 🇷🇺", u8"EN 🇬🇧");
    TgBot::InlineKeyboardMarkup::Ptr keyboard_gen(new TgBot::InlineKeyboardMarkup);
    MakeTwoButtonsKeyboard( keyboard_gen, u8"YES ✅", u8"NO ❌" );

    bool language_flag = true;
    bool number_input_flag = false;
    bool word_input_flag = false;
    int word_num = 0;

    std::string entered_words;
    std::vector<std::string> words;
    sqlite3* db;

    bot.getEvents().onUnknownCommand( [&bot, &number_input_flag, &word_input_flag, &words, &word_num, &language_flag]( TgBot::Message::Ptr message ) 
    {
        BadEnter( words, word_num, word_input_flag );
        number_input_flag = false;

        if ( language_flag )
            IncorrectMessages( bot, message, u8"Unknown Command.", language_flag );
        else
            IncorrectMessages( bot, message, u8"Неизвестная команда.", language_flag );
    } );

    bot.getEvents().onCommand( "instruction", [&bot, &number_input_flag, &word_input_flag, &words, &word_num, &language_flag]( TgBot::Message::Ptr message ) 
    {
        BadEnter(words, word_num, word_input_flag);
        number_input_flag = false;
        Instruction(bot, message, language_flag);
    } );

    bot.getEvents().onCommand( "start", [&bot, &number_input_flag, &keyboard_lang, &word_input_flag, &words, &word_num, &language_flag, &db]( TgBot::Message::Ptr message )
    {
        const std::string QUERY(
            "CREATE TABLE IF NOT EXISTS USERS("
            "USER_ID	INTEGER		NOT NULL,"
            "UNIQUE(USER_ID)"
            ");"
            "INSERT OR IGNORE INTO USERS(USER_ID) VALUES(" + std::to_string( message->from->id ) + ");"
            "SELECT COUNT(*) from USERS; " );

        sqlite3_open( "USERS.db", &db );
        sqlite3_exec( db, QUERY.c_str(), callback, 0, nullptr );
        sqlite3_close( db );
    
        BadEnter( words, word_num, word_input_flag );
        number_input_flag = false;
    
        std::string message_text;
        if ( language_flag )
            message_text = u8"Hi 👋 " + message->chat->firstName + u8"! Choose the language in which it is convenient for you to communicate with me.";
        else
            message_text = u8"Привет 👋 " + message->chat->firstName + u8"! Выберите язык на котором вам удобнее со мной общаться.";

        bot.getApi().sendMessage( message->chat->id, message_text, false, 0, keyboard_lang );
    });

    bot.getEvents().onCallbackQuery( [&bot, &number_input_flag, &language_flag, &word_input_flag, &words, &word_num]( TgBot::CallbackQuery::Ptr query ) 
    {
        number_input_flag = false;

        if ( query->data == u8"NO ❌" || query->data == u8"YES ✅" ) 
        {
            if ( word_input_flag ) 
            {
                if ( language_flag )
                    bot.getApi().sendMessage( query->message->chat->id, u8"You didn't enter all the words. ✉️" );
                else
                    bot.getApi().sendMessage( query->message->chat->id, u8"Вы ввели не все слова. ✉️" );
            }
            else if ( query->data == u8"NO ❌" )
            {
                BadEnter( words, word_num, word_input_flag );
                if ( language_flag )
                    IncorrectMessages( bot, query->message, u8"Words dropped.", language_flag );
                else
                    IncorrectMessages( bot, query->message, u8"Слова сброшены.", language_flag );
            }
            else if ( words.size() ) 
            {
                if ( language_flag )
                    bot.getApi().sendMessage( query->message->chat->id, u8"Words accepted! Expect a crossword puzzle! This may take up to 5 minutes. ⏰" );
                else
                    bot.getApi().sendMessage( query->message->chat->id, u8"Слова приняты! Начинаю состовлять кроссворд! Это может занять до 5 минут. ⏰" );
    
                GenerateCrossword( words, bot, query->message, language_flag );
                words.clear();
            }
            return;
        }

        BadEnter(words, word_num, word_input_flag);
    
        if ( query->data == u8"RU 🇷🇺" )
        {
            bot.getApi().sendMessage( query->message->chat->id, u8"Выбран русский 🇷🇺" );
            language_flag = false;
            Instruction( bot, query->message, language_flag );
        }
        else if ( query->data == u8"EN 🇬🇧" )
        {
            bot.getApi().sendMessage( query->message->chat->id, u8"Selected English 🇬🇧" );
            language_flag = true;
            Instruction( bot, query->message, language_flag );
        }
    } );

    bot.getEvents().onCommand( "generate", [&bot, &number_input_flag, &words, &word_num, &word_input_flag, &language_flag]( TgBot::Message::Ptr message )
    {
        BadEnter( words, word_num, word_input_flag );
        if ( language_flag )
            bot.getApi().sendMessage( message->chat->id, u8"Enter number of words (2 - 20):" );
        else
            bot.getApi().sendMessage( message->chat->id, u8"Введите число слов (2 - 20):" );
        number_input_flag = true;
    } );

    bot.getEvents().onNonCommandMessage( [&bot, &number_input_flag, &word_num, &word_input_flag, &words, &keyboard_gen, &entered_words, &language_flag]( TgBot::Message::Ptr message ) 
    {
        if ( word_input_flag )
        {
            if ( !CheckWord( message, language_flag ) )
            {
                if ( language_flag )
                    bot.getApi().sendMessage( message->chat->id, u8"❌ The word does not match the requirements, try to enter a new one:" );
                else
                    bot.getApi().sendMessage( message->chat->id, u8"❌ Слово не соответствует требованиям, попробуйте ввести новое:" );
                return;
            }

            words.push_back( message->text );
            --word_num;

            if ( !word_num )
            {
                word_input_flag = false;
    
                for ( const auto & word : words )
                    entered_words += word + '\n';
    
                if ( language_flag )
                    bot.getApi().sendMessage( message->chat->id, entered_words + u8"Are all words entered correctly?", false, 0, keyboard_gen );
                else
                    bot.getApi().sendMessage( message->chat->id, entered_words + u8"Все слова введены правильно?", false, 0, keyboard_gen );
                entered_words.clear();
            }
            return;
        }
        try {
            if ( number_input_flag )
            {
                word_num = std::stoi( message->text );
    
                if ( word_num < 1 || word_num > 20 )
                    throw std::invalid_argument( message->text );
    
                if ( language_flag )
                    bot.getApi().sendMessage( message->chat->id, u8"⚡ Enter " + message->text + u8" words from which I will generate a crossword puzzle for you:" );
                else
                    bot.getApi().sendMessage( message->chat->id, u8"⚡ Введите " + message->text + u8" слов, из которых я сгенерирую кроссворд для вас:" );
    
                number_input_flag = false;
                word_input_flag = true;
                return;
            }
            else
                throw std::invalid_argument( message->text );
        }
        catch ( std::invalid_argument & ) 
        {
            word_num = 0;
            number_input_flag = false;
    
            if ( language_flag )
                IncorrectMessages( bot, message, u8"Incorrest message.", language_flag );
            else
                IncorrectMessages( bot, message, u8"Неверное сообщение.", language_flag );
            return;
        }
    } );

    bot.getEvents().onCommand( "users_count", [&bot]( TgBot::Message::Ptr message ) 
    {
        if ( users_count == -1 )
            return;
        bot.getApi().sendMessage( message->chat->id, std::to_string( users_count ) );
    } );

    try 
    {
        printf( "Bot username: %s\n", bot.getApi().getMe()->username.c_str() );
        TgBot::TgLongPoll longPoll( bot );
        while ( true ) 
        {
            try 
            {
                printf( "Long poll started\n" );
                longPoll.start();
            }
            catch ( TgBot::TgException & e ) 
            {
                std::string one( e.what() ), two( "Forbidden: bot was blocked by the user" );
                if ( one == two ) 
                {
                    for ( auto & up : bot.getApi().getUpdates() )
                        up = nullptr;
                }
                else 
                    throw;
            }
        }
    }
    catch ( TgBot::TgException & e ) 
    {
        printf( "error: %s\n", e.what() );
    }
    return 0;
}
