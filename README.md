Требования к проекту
====================
Введение
--------
Данный проект имеет название “Password Manager”. Он предназначен для пользователей, у которых есть множество регистраций на разных сайтах и, в свою очередь, логинов и паролей на них.

Продукт будет позволять удаленное хранение логинов и паролей пользователей в зашифрованном виде на сервере, к которым возможен доступ путем авторизации в данной программе.

Требования пользователя
-----------------------
### Программные интерфейсы

Проект потребует использования протокола TCP для передачи данных между сервером и пользователем для получения нужной информации с сервера.

Также проект будет использовать метод шифрации данных с ключом, которым будет являться слово, сгенерированное путем скрещения личных данных при создании аккаунта для данного приложения.

Кроме этого потребуется стороння библиотека Qt для создания удобной и функциональной GUI оболочки, с которой сможет легко обращаться любой пользователь.
