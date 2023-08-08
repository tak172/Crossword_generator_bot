<p align="center">
      <img src="https://i.ibb.co/6XWhQY9/logo.png" width="300">
</p>

<p align="center">
   <img src="https://img.shields.io/badge/Visual_Studio_2022-ISO_C%2B%2B_2020-yellow?logo=VisualStudio" alt="Standart">
   <img src="https://img.shields.io/badge/Crossword_generator_bot-v1.0-yellow" alt="Version">
   <img src="https://img.shields.io/badge/GNU%20General-Public%20License-yellow" alt="License">
</p>

## About

Crossword Generator bot поможет пользователю составить кроссворд из списка слов (до 20). Бот пытается составить кроссворд с наибольшим количеством пересечений. Алгоритм работает довольно быстро по сравнению с другими, которые я рассмотрел. Поддерживается 2 языка - английский и русский.

## Start

Для начала вам необходимо создать бота. На habr лежит подробная [статья](https://habr.com/ru/articles/543676/) обо всем, что необходимо знать на первом этапе.

Далее нужно установить необходимые библиотеки. Инструкция по установке tgbot-cpp находится [здесь](https://github.com/reo7sp/tgbot-cpp#tgbot-cpp). Если вы работаете на Windows и устанавливали библиотеку tgbot-cpp через [vspkg](https://github.com/microsoft/vcpkg), то через него же вы можете установить sqlite3.

После создания проекта и проверки подключения библиотек, для совместимости кодировок (русский язык) необходимо указать в дополнительных параметрах командной строки **/utf-8**. В Visual Studio необходимо перейти в **Проект - Свойства - С/С++ - Командная строка** и в **Дополнительных параметрах** написать **/utf-8**.

<p align="center"><img src="https://i.ibb.co/yytqbpT/image.png" width="500">
</p>

Затем необходимо сохранить bot.cpp с кодировкой "Юникод (UTF-8, с сигнатурой).

<p align="center">
<img src="https://i.ibb.co/3MPF8sZ/image.png" width="300">
</p>

Последний шаг перед запуском, скопируйте токен вашего бота у BotFather(используете команду /mybots, выбираете бота и нажимаете APItoken), создайте в папке с проектом token.txt, вставьте токен в файл.

Готово! Запускайте проект и пользуйтесь ботом.

Для того чтобы ваш бот стабильно работал, его необходимо разместить на хостинге. Есть платные варианты (гарантируют стабильную работу), и бесплатные. К сожалению у меня возникли трудности с размещением бота на бесплатных хостин сервисах, а платные я не пробовал, в связи с чем не могу ничего порекомендовать.

## Commands

- **/instruction** - инструкция по использованию бота.
- **/start** - запуск бота и выбор языка
- **/generate** - начало генерации кроссворда
- **/users_count** - количество пользователей бота

## Demonstration

Видео демонстрации работы:

https://github.com/tak172/Crossword_generator_bot/assets/120005445/4a01198e-5630-4807-826d-4a18b35d0761

## Documentation

- [SQLite](https://www.sqlite.org/cintro.html);
- [tgbot-cpp](https://reo7sp.github.io/tgbot-cpp/);

## Developers

- [Kromachev Timofey](https://github.com/tak172)
- [Kromachev Maxim](https://github.com/kromachmax)

## License
Project Crossword_Generator_bot is distributed under the **GNU General Public License**.
