-- phpMyAdmin SQL Dump
-- version 3.5.0
-- http://www.phpmyadmin.net
--
-- Хост: localhost
-- Время создания: Дек 07 2016 г., 23:22
-- Версия сервера: 5.5.53
-- Версия PHP: 5.3.10

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- База данных: `gibdd`
--

DELIMITER $$
--
-- Процедуры
--
CREATE DEFINER=`root`@`localhost` PROCEDURE `accident_info`(type int, x date, y date, str varchar(20))
BEGIN
	SELECT `accident_date` as `Дата ДТП`, CASE `place_category_id` WHEN 1 THEN `street` ELSE `Route_name` END as `Улица (Трасса)`, CASE `place_category_id` WHEN 1 THEN `house_number` ELSE `route_km` END as `Дом (Километр трассы)`, `type_of_accident` as `Тип ДТП`, `accident_description` as `Описание ДТП`, `cause_of_accident` as `Причина ДТП`, `Road_conditions` as `Дорожные условия`
FROM `road_accident` as `a`, `type_of_accident` as `t`, `place_of_accident` as `p` LEFT JOIN `settlement` as `s` ON `p`.`place_id` = `s`.`place_id` LEFT JOIN `section_of_the_route` as `sr` ON `p`.`place_id` = `sr`.`place_id` 
WHERE `a`.`place_id` = `p`.`place_id`
AND `a`.`type_of_accident_id` = `t`.`type_of_accident_id`
AND (`street` LIKE coalesce(str, "%") OR `route_name` LIKE coalesce(str, "%"))
AND CASE type WHEN 1 THEN (`a`.`type_of_accident_id` = 1) WHEN 2 THEN (`a`.`type_of_accident_id` = 2) WHEN 3 THEN (`a`.`type_of_accident_id` = 3) WHEN 4 THEN (`a`.`type_of_accident_id` = 4) WHEN 5 THEN (`a`.`type_of_accident_id` = 5) ELSE (`a`.`type_of_accident_id` IS NOT NULL) END
AND `a`.`accident_date` BETWEEN coalesce(x, "0001-01-01") AND coalesce(y, NOW());
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `add_accident`(plcat_id int, strrou varchar (20), houkm int, data date, rc varchar(20), tod varchar(10), acdesc varchar (200), coa varchar (40), typeid int, OUT c int)
BEGIN
	DECLARE a INT;
        DECLARE b INT;
        CALL `place_check` (strrou, houkm, b);
        IF b IS NULL THEN
        	INSERT INTO `place_of_accident` (`place_category_id`) VALUES (plcat_id);
       		SELECT LAST_INSERT_ID() INTO a;
        	IF plcat_id = 1 THEN
        		INSERT INTO `settlement` (`place_id`, `street`, `house_number`) VALUES (a, strrou, houkm); 
        	ELSE
                	INSERT INTO `section_of_the_route` (`place_id`, `route_name`, `route_km`) VALUES (a, strrou, houkm);
                END IF;
                INSERT INTO `road_accident` (`accident_date`, `road_conditions`, `time_of_day`, `accident_description`, `cause_of_accident`, `accident_scheme`, `type_of_accident_id`, `place_id`) VALUES (data, rc, tod, acdesc, coa, NULL, typeid, a);
        ELSE
        	INSERT INTO `road_accident` (`accident_date`, `road_conditions`, `time_of_day`, `accident_description`, `cause_of_accident`, `accident_scheme`, `type_of_accident_id`, `place_id`) VALUES (data, rc, tod, acdesc, coa, NULL, typeid, b);
         END IF;
    		SELECT LAST_INSERT_ID() INTO c; 
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `add_hijacking`(hjdate date, wayid int, status int, vehicleid int, street varchar(30), house int)
BEGIN
	DECLARE a INT;
        INSERT INTO `place_of_accident` (`place_category_id`) VALUES (1);
        SELECT LAST_INSERT_ID() INTO a;
	INSERT INTO `settlement` (`place_id`, `street`, `house_number`) VALUES (a, street, house);
        INSERT INTO `vehicle_hijacking` (`hijacking_date`, `hijacking_way_id`, `status_id`, `vehicle_id`, `place_id`, `status_date`) VALUES (hjdate, wayid, status, vehicleid, a, NOW());
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `add_individual`(phonenumber varchar(20), fullname varchar(50), regadress varchar(50))
BEGIN
	DECLARE a INT;
	INSERT INTO `vehicle_owner` (`phone_number`, `owner_category_id`) VALUES (phonenumber, 1);
        SELECT LAST_INSERT_ID() INTO a; 
        INSERT INTO `individual` VALUES (fullname, regadress, a);
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `add_legalentity`(phonenumber varchar(20), companyname varchar(50), legaladress varchar(50), physadress varchar(50), chief varchar(40))
BEGIN
	DECLARE a INT;
	INSERT INTO `vehicle_owner` (`phone_number`, `owner_category_id`) VALUES (phonenumber, 2);
        SELECT LAST_INSERT_ID() INTO a; 
        INSERT INTO `legal_entity` VALUES (companyname, legaladress, physadress, chief, a);
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `hijacking_info`(str varchar(20), x date, y date, status int)
BEGIN
	SELECT  `hijacking_date` AS  `Дата угона` ,  `Brand_name` AS  `Транспортное средство` ,  `street` AS  `Улица` ,  `House_number` AS  `Номер дома` ,  `Way_name` AS  `Способ угона` ,  `Status_name` AS  `Статус` ,  `Status_date` AS `Дата статуса` 
FROM  `vehicle_hijacking` AS  `h` 
INNER JOIN  `vehicle` AS  `v` ON  `v`.`vehicle_id` =  `h`.`vehicle_id` ,  `way_of_hijacking` AS  `w` ,  `vehicle_status` AS  `st` ,  `place_of_accident` AS  `p` 
LEFT JOIN  `settlement` AS  `s` ON  (`p`.`place_id` =  `s`.`place_id`) 
WHERE  `h`.`vehicle_id` =  `v`.`vehicle_id` 
AND  `w`.`hijacking_way_id` =  `h`.`hijacking_way_id` 
AND  `h`.`status_id` =  `st`.`status_id` 
AND  `p`.`place_id` =  `h`.`place_id`
AND CASE status WHEN 1 THEN (`h`.`status_id` = 1) WHEN 2 THEN (`h`.`status_id` = 2) WHEN 3 THEN (`h`.`status_id` = 3) ELSE (`h`.`status_id` IS NOT NULL) END
AND `street` LIKE coalesce(str, "%")
AND  `h`.`hijacking_date` 
BETWEEN coalesce(x, "0001-01-01") AND coalesce(y, NOW());
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `place_check`(strrou varchar (20), houkm int, OUT x int)
BEGIN
       DECLARE a int;
       SET a = IF (EXISTS (SELECT * FROM `settlement`, `section_of_the_route` WHERE (strrou = `street` AND `houkm` = `house_number`) OR (strrou = `route_name` AND houkm = `route_km`)), 1, 2);
       CASE a WHEN 1 THEN (SELECT `p`.`place_id` INTO x FROM  `place_of_accident` AS  `p` 
	LEFT JOIN  `settlement` AS  `s` ON  `p`.`place_id` =  `s`.`place_id` 
	LEFT JOIN  `section_of_the_route` AS  `sr` ON  `p`.`place_id` =  `sr`.`place_id` WHERE (strrou = `street` AND `houkm` = `house_number`) OR (strrou = `route_name` AND houkm = `route_km`)); ELSE SET x = NULL; END case;
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `registration_info`(tag varchar(10), x date, y date, status int, type int)
BEGIN
	SELECT  `vehicle_registration_tag` AS  `Регистрационный номер` ,  `Registration_date` AS  `Дата регистрации` ,  `Removal_date` AS  `Дата_снятия с учёта` ,  `Brand_name` AS  `Транспортное_средство` , 
CASE  `o`.`owner_category_id` 
WHEN 1 
THEN  `full_name` 
ELSE  `company_name` 
END AS  `Владелец` 
FROM  `vehicle_registration` AS  `r` ,  `vehicle_registration_tag` AS  `t` ,  `vehicle` AS  `v` ,  `vehicle_owner` AS  `o` 
LEFT JOIN  `individual` AS  `i` ON  `o`.`owner_id` =  `i`.`owner_id` 
LEFT JOIN  `legal_entity` AS  `l` ON  `o`.`owner_id` =  `l`.`owner_id` 
WHERE  `r`.`tag_id` =  `t`.`tag_id` AND `vehicle_registration_tag` LIKE coalesce(tag, "%")
AND  `r`.`vehicle_id` =  `v`.`vehicle_id`
AND CASE type WHEN 1 THEN (`Type_id` = 1) WHEN 2 THEN (`Type_id` = 2) WHEN 3 THEN (`Type_id` = 3) ELSE (`Type_id` IS NOT NULL) END
AND CASE status WHEN 1 THEN (`Removal_date` IS NOT NULL) WHEN 2 THEN (`Removal_date` IS NULL) ELSE (`Removal_date` IS NULL OR `Removal_date` IS NOT NULL) END
AND  `r`.`owner_id` =  `o`.`owner_id` AND `Registration_date` BETWEEN coalesce(x, "0001-01-01") AND coalesce(y, NOW());
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Структура таблицы `car_transport`
--

CREATE TABLE IF NOT EXISTS `car_transport` (
  `Body_type` varchar(20) DEFAULT NULL,
  `Vehicle_ID` int(11) NOT NULL,
  PRIMARY KEY (`Vehicle_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `car_transport`
--

INSERT INTO `car_transport` (`Body_type`, `Vehicle_ID`) VALUES
('седан', 1),
('седан', 2),
('седан', 5),
('седан', 6),
('седан', 7),
('внедорожник', 10),
('универсал', 11),
('хетчбэк', 16),
('внедорожник', 17),
('седан ', 18),
('внедорожник', 19),
('седан ', 20);

-- --------------------------------------------------------

--
-- Структура таблицы `extent_of_participation`
--

CREATE TABLE IF NOT EXISTS `extent_of_participation` (
  `Extent_ID` int(11) NOT NULL,
  `Extent` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`Extent_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `extent_of_participation`
--

INSERT INTO `extent_of_participation` (`Extent_ID`, `Extent`) VALUES
(1, 'виновник'),
(2, 'пострадавший');

-- --------------------------------------------------------

--
-- Структура таблицы `freight_transport`
--

CREATE TABLE IF NOT EXISTS `freight_transport` (
  `Unloading_mode` varchar(20) DEFAULT NULL,
  `Vehicle_ID` int(11) NOT NULL,
  PRIMARY KEY (`Vehicle_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `freight_transport`
--

INSERT INTO `freight_transport` (`Unloading_mode`, `Vehicle_ID`) VALUES
('самосвал', 3),
('ручная', 8),
('самосвал', 9),
('автокран', 12),
('ручная', 13);

-- --------------------------------------------------------

--
-- Структура таблицы `individual`
--

CREATE TABLE IF NOT EXISTS `individual` (
  `Full_name` varchar(60) DEFAULT NULL,
  `Registration_adress` varchar(60) DEFAULT NULL,
  `Owner_ID` int(11) NOT NULL,
  PRIMARY KEY (`Owner_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `individual`
--

INSERT INTO `individual` (`Full_name`, `Registration_adress`, `Owner_ID`) VALUES
('Макарычев Антон Игоревич', 'г. Пенза, ул. Свердлова, д. 62, кв. 47', 1),
('Прокопенко Леонид Степанович', 'г. Пенза, ул. Астраханская, д. 58, кв. 1', 3),
('Кутепов Вадим Алексеевич', 'г. Пенза, ул. Плеханова, д. 85, кв. 107', 5),
('Регенд Александр Робертович', 'г. Пенза, пр-т Победы 121, кв. 124', 6),
('Пронин Алексей Сергеевич', 'г. Пенза, ул. Кавказ, д. 76, кв.1', 7),
('Павлов Сергей Сергеевич', 'г. Пенза, ул. Антонова, д. 5, кв. 30', 9),
('Артамонов Кирилл Андреевич', 'г. Пенза, ул. Бекешская, д. 82, кв. 37', 10),
('Богатов Виктор Вячеславович', 'г. Пенза, ул. Карла Маркса, д. 12, кв. 71', 12),
('Шамшилов Даниил Валерьевич', 'г. Пенза, ул. Урицкого, д. 117, кв. 7', 14);

-- --------------------------------------------------------

--
-- Структура таблицы `legal_entity`
--

CREATE TABLE IF NOT EXISTS `legal_entity` (
  `Company_name` varchar(40) DEFAULT NULL,
  `Legal_adress` varchar(60) DEFAULT NULL,
  `Physical_adress` varchar(60) DEFAULT NULL,
  `Chief` varchar(60) DEFAULT NULL,
  `Owner_ID` int(11) NOT NULL,
  PRIMARY KEY (`Owner_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `legal_entity`
--

INSERT INTO `legal_entity` (`Company_name`, `Legal_adress`, `Physical_adress`, `Chief`, `Owner_ID`) VALUES
('ИП Фёдоров', 'г. Пенза, ул. Лядова, д. 118а', 'г. Пенза, ул. Московская, д. 77', 'Фёдоров Александр Владимирович', 2),
('ПАО Сбербанк', 'г. Пенза, ул. Московская, д. 13', 'г. Пенза, ул. Клары Цеткин, д. 23', 'Владимиров Владимир Павлович', 4),
('МУП Пензадормост', 'г. Пенза, ул. Ленина, д. 2', 'г. Пенза, ул. Ленина, д. 2', 'Князихин Александр Сергеевич', 8),
('ЗАО Пензрегионсбыт', 'г. Пенза, ул. Свободы, д. 83', 'г. Пенза, ул. Свободы, д. 83', 'Маро Алексей Юрьевич', 11),
('ИП Дзахмишев', 'г. Пенза, ул. Ленина, д. 8', 'г. Пенза, ул. Нейтральная, д. 54', 'Дзахмишев Рамзан Шамильевич', 13),
('ООО Тантал', 'г. Пенза, ул. Ворошилова, д. 7', 'г. Пенза, ул. Ворошилова, д. 7', 'Игнатов Вадим Алексеевич', 15);

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `owners`
--
CREATE TABLE IF NOT EXISTS `owners` (
`ID Владельца` int(11)
,`Телефон` varchar(20)
,`Категория владельца` int(11)
,`ФИО` varchar(60)
,`Название компании` varchar(40)
);
-- --------------------------------------------------------

--
-- Структура таблицы `owner_category`
--

CREATE TABLE IF NOT EXISTS `owner_category` (
  `Owner_category_ID` int(11) NOT NULL,
  `Owner_category` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`Owner_category_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `owner_category`
--

INSERT INTO `owner_category` (`Owner_category_ID`, `Owner_category`) VALUES
(1, 'физическое лицо'),
(2, 'юридическое лицо');

-- --------------------------------------------------------

--
-- Структура таблицы `participant_category`
--

CREATE TABLE IF NOT EXISTS `participant_category` (
  `Participant_category_ID` int(11) NOT NULL,
  `Participant_category` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`Participant_category_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `participant_category`
--

INSERT INTO `participant_category` (`Participant_category_ID`, `Participant_category`) VALUES
(1, 'незарегестрированное ТС'),
(2, 'зарегестрированное ТС'),
(3, 'пешеход');

-- --------------------------------------------------------

--
-- Структура таблицы `participant_of_accident`
--

CREATE TABLE IF NOT EXISTS `participant_of_accident` (
  `Participant_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Participant_category_ID` int(11) NOT NULL,
  PRIMARY KEY (`Participant_ID`),
  KEY `R_66` (`Participant_category_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=21 ;

--
-- Дамп данных таблицы `participant_of_accident`
--

INSERT INTO `participant_of_accident` (`Participant_ID`, `Participant_category_ID`) VALUES
(2, 1),
(7, 1),
(8, 1),
(12, 1),
(15, 1),
(1, 2),
(4, 2),
(5, 2),
(9, 2),
(10, 2),
(11, 2),
(13, 2),
(14, 2),
(16, 2),
(17, 2),
(18, 2),
(19, 2),
(20, 2),
(3, 3),
(6, 3);

-- --------------------------------------------------------

--
-- Структура таблицы `participation_in_accident`
--

CREATE TABLE IF NOT EXISTS `participation_in_accident` (
  `Loss` int(11) DEFAULT NULL,
  `Extent_ID` int(11) NOT NULL,
  `Accident_ID` int(11) NOT NULL,
  `Participant_ID` int(11) NOT NULL,
  `Record_ID` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`Record_ID`),
  KEY `R_4` (`Extent_ID`),
  KEY `R_42` (`Accident_ID`),
  KEY `R_56` (`Participant_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=19 ;

--
-- Дамп данных таблицы `participation_in_accident`
--

INSERT INTO `participation_in_accident` (`Loss`, `Extent_ID`, `Accident_ID`, `Participant_ID`, `Record_ID`) VALUES
(50000, 2, 1, 1, 1),
(40000, 1, 9, 8, 2),
(130000, 2, 10, 7, 3),
(160000, 1, 10, 2, 4),
(20000, 2, 8, 6, 5),
(5000, 1, 8, 7, 6),
(100000, 1, 7, 4, 7),
(100000, 1, 1, 12, 8),
(45000, 1, 6, 13, 9),
(30000, 2, 6, 14, 10),
(50000, 2, 5, 3, 11),
(15000, 1, 5, 11, 12),
(80000, 1, 4, 5, 13),
(180000, 2, 4, 9, 14),
(135000, 2, 4, 10, 15),
(50000, 2, 3, 18, 16),
(95000, 1, 3, 17, 17),
(70000, 1, 2, 15, 18);

-- --------------------------------------------------------

--
-- Структура таблицы `passenger_transport`
--

CREATE TABLE IF NOT EXISTS `passenger_transport` (
  `Number_of_seats` int(11) DEFAULT NULL,
  `Standing_place_number` int(11) DEFAULT NULL,
  `Vehicle_ID` int(11) NOT NULL,
  PRIMARY KEY (`Vehicle_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `passenger_transport`
--

INSERT INTO `passenger_transport` (`Number_of_seats`, `Standing_place_number`, `Vehicle_ID`) VALUES
(31, 45, 4),
(18, 3, 14),
(24, 5, 15);

-- --------------------------------------------------------

--
-- Структура таблицы `pedestrian`
--

CREATE TABLE IF NOT EXISTS `pedestrian` (
  `Full_name` varchar(4000) DEFAULT NULL,
  `Residence_adress` varchar(4000) DEFAULT NULL,
  `Phone_number` varchar(20) DEFAULT NULL,
  `Participant_ID` int(11) NOT NULL,
  PRIMARY KEY (`Participant_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `pedestrian`
--

INSERT INTO `pedestrian` (`Full_name`, `Residence_adress`, `Phone_number`, `Participant_ID`) VALUES
('Аверин Максим Михайлович', 'г. Пенза, ул. Мира, д. 89, кв. 7', '89022875490', 3),
('Тряшунин Алексей Михайлович', 'г. Пенза, ул. Коннозаводская, д. 57', '89638902110', 6);

-- --------------------------------------------------------

--
-- Структура таблицы `place_category`
--

CREATE TABLE IF NOT EXISTS `place_category` (
  `Place_category_ID` int(11) NOT NULL,
  `Place_category` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`Place_category_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `place_category`
--

INSERT INTO `place_category` (`Place_category_ID`, `Place_category`) VALUES
(1, 'населенный пункт'),
(2, 'участок трассы');

-- --------------------------------------------------------

--
-- Структура таблицы `place_of_accident`
--

CREATE TABLE IF NOT EXISTS `place_of_accident` (
  `Place_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Place_category_ID` int(11) DEFAULT NULL,
  PRIMARY KEY (`Place_ID`),
  KEY `R_65` (`Place_category_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=23 ;

--
-- Дамп данных таблицы `place_of_accident`
--

INSERT INTO `place_of_accident` (`Place_ID`, `Place_category_ID`) VALUES
(1, 1),
(2, 1),
(3, 1),
(5, 1),
(7, 1),
(9, 1),
(12, 1),
(13, 1),
(14, 1),
(15, 1),
(16, 1),
(19, 1),
(21, 1),
(22, 1),
(4, 2),
(6, 2),
(8, 2),
(10, 2),
(11, 2),
(17, 2),
(18, 2),
(20, 2);

-- --------------------------------------------------------

--
-- Структура таблицы `registered_vehicle`
--

CREATE TABLE IF NOT EXISTS `registered_vehicle` (
  `Participant_ID` int(11) NOT NULL,
  `Vehicle_ID` int(11) NOT NULL,
  PRIMARY KEY (`Participant_ID`),
  KEY `R_61` (`Vehicle_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `registered_vehicle`
--

INSERT INTO `registered_vehicle` (`Participant_ID`, `Vehicle_ID`) VALUES
(4, 3),
(20, 4),
(19, 5),
(1, 6),
(10, 7),
(5, 11),
(9, 14),
(18, 15),
(11, 16),
(16, 17),
(13, 18),
(17, 19),
(14, 20);

--
-- Триггеры `registered_vehicle`
--
DROP TRIGGER IF EXISTS `accident_status`;
DELIMITER //
CREATE TRIGGER `accident_status` BEFORE INSERT ON `registered_vehicle`
 FOR EACH ROW BEGIN
  UPDATE `vehicle_registration` SET `accident_status` = 1 WHERE NEW.`vehicle_id` = `vehicle_registration`.`vehicle_id`; 
END
//
DELIMITER ;

-- --------------------------------------------------------

--
-- Структура таблицы `road_accident`
--

CREATE TABLE IF NOT EXISTS `road_accident` (
  `Accident_date` date DEFAULT NULL,
  `Road_conditions` varchar(40) DEFAULT NULL,
  `Time_of_day` varchar(20) DEFAULT NULL,
  `Accident_description` varchar(500) DEFAULT NULL,
  `Cause_of_accident` varchar(40) DEFAULT NULL,
  `Accident_scheme` blob,
  `Accident_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Type_of_accident_ID` int(11) NOT NULL,
  `Place_ID` int(11) NOT NULL,
  PRIMARY KEY (`Accident_ID`),
  KEY `R_89` (`Type_of_accident_ID`),
  KEY `R_33` (`Place_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=12 ;

--
-- Дамп данных таблицы `road_accident`
--

INSERT INTO `road_accident` (`Accident_date`, `Road_conditions`, `Time_of_day`, `Accident_description`, `Cause_of_accident`, `Accident_scheme`, `Accident_ID`, `Type_of_accident_ID`, `Place_ID`) VALUES
('2016-08-19', 'дождь', 'ночь', 'лобовое столкновение двух автомобилей', 'нарушение ПДД', NULL, 1, 4, 6),
('2016-06-07', 'туман, дождь', 'утро', 'столкновение с опорой путепровода', 'невнимательное вождение', NULL, 2, 2, 10),
('2016-08-29', 'дождь', 'вечер', 'боковое столкновение двух ТС', 'вождение в нетрезвом виде', NULL, 3, 4, 13),
('2015-11-03', 'ясно', 'ночь', 'столкновение трех ТС', 'нарушение ПДД', NULL, 4, 4, 18),
('2016-09-01', 'дождь', 'день', 'наезд на пешехода на переходе', 'превышение скорости', NULL, 5, 1, 19),
('2016-01-18', 'снег', 'ночь', 'заднее столкновение двух ТС', 'несоблюдение дистанции', NULL, 6, 4, 15),
('2016-04-03', 'дождь', 'день', 'опрокидывание на обочину', 'превышение скорости', NULL, 7, 3, 20),
('2015-07-07', 'ясно', 'день', 'наезд на пешехода', 'вождение в нетрезвом виде', NULL, 8, 1, 14),
('2016-07-10', 'туман', 'утро', 'столкновение со столбом', 'вождение в нетрезвом виде', NULL, 9, 2, 12),
('2016-10-09', 'ясно', 'ночь', 'лобовое столкновение двух ТС', 'вождение в нетрезвом виде', NULL, 10, 4, 5),
('2016-12-07', 'lol', 'lol', 'lol', 'lol', NULL, 11, 2, 3);

-- --------------------------------------------------------

--
-- Структура таблицы `section_of_the_route`
--

CREATE TABLE IF NOT EXISTS `section_of_the_route` (
  `Place_ID` int(11) NOT NULL,
  `Route_name` varchar(30) DEFAULT NULL,
  `Route_km` int(11) DEFAULT NULL,
  PRIMARY KEY (`Place_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `section_of_the_route`
--

INSERT INTO `section_of_the_route` (`Place_ID`, `Route_name`, `Route_km`) VALUES
(4, 'М5-Урал', 278),
(6, 'М5-Урал', 617),
(8, 'Оренбург-Самара', 320),
(10, 'М5-Урал', 693),
(11, 'Р-124', 54),
(17, 'Москва-Саратов', 453),
(18, 'М5-Урал', 642),
(20, 'Р-124', 128);

-- --------------------------------------------------------

--
-- Структура таблицы `settlement`
--

CREATE TABLE IF NOT EXISTS `settlement` (
  `Place_ID` int(11) NOT NULL,
  `Street` varchar(30) DEFAULT NULL,
  `House_number` int(11) DEFAULT NULL,
  PRIMARY KEY (`Place_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `settlement`
--

INSERT INTO `settlement` (`Place_ID`, `Street`, `House_number`) VALUES
(1, 'Бекешская', 75),
(2, 'Антонова', 6),
(3, 'Суворова', 123),
(5, 'Пушкина', 80),
(7, 'Ворошилова', 39),
(9, 'Аустрина', 92),
(12, 'Чапаева', 69),
(13, 'Мира', 24),
(14, 'пр-т Строителей', 62),
(15, 'пр-т Победы', 127),
(16, 'Чаадаева', 40),
(19, 'Гагарина', 82),
(21, 'Воронова', 79),
(22, 'Пушкина', 142);

-- --------------------------------------------------------

--
-- Структура таблицы `sort_of_number`
--

CREATE TABLE IF NOT EXISTS `sort_of_number` (
  `Sort_of_number_ID` int(11) NOT NULL,
  `Sort_of_number` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`Sort_of_number_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `sort_of_number`
--

INSERT INTO `sort_of_number` (`Sort_of_number_ID`, `Sort_of_number`) VALUES
(1, 'особая серия (ФСБ, ФСО, Правительство и пр.)'),
(2, 'номер вида -XXX--'),
(3, 'номер вида -X00--'),
(4, 'номер вида X---XX'),
(5, 'номер вида -00X--'),
(6, 'обычный номер');

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `stolen_vehicle`
--
CREATE TABLE IF NOT EXISTS `stolen_vehicle` (
`ID ТС` int(11)
,`Марка_ТС` varchar(40)
);
-- --------------------------------------------------------

--
-- Структура таблицы `type_of_accident`
--

CREATE TABLE IF NOT EXISTS `type_of_accident` (
  `Type_of_accident_ID` int(11) NOT NULL,
  `Type_of_accident` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`Type_of_accident_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `type_of_accident`
--

INSERT INTO `type_of_accident` (`Type_of_accident_ID`, `Type_of_accident`) VALUES
(1, 'наезд на пешехода'),
(2, 'наезд на препятствие'),
(3, 'опрокидывание/падение'),
(4, 'столкновение автомобилей'),
(5, 'столкновение с животным');

-- --------------------------------------------------------

--
-- Структура таблицы `unregistered_vehicle`
--

CREATE TABLE IF NOT EXISTS `unregistered_vehicle` (
  `Participant_ID` int(11) NOT NULL,
  `Vehicle_registration_tag` varchar(10) DEFAULT NULL,
  `Vehicle_brand` varchar(40) DEFAULT NULL,
  `VIN_code` varchar(30) DEFAULT NULL,
  `Vehicle_color` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`Participant_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `unregistered_vehicle`
--

INSERT INTO `unregistered_vehicle` (`Participant_ID`, `Vehicle_registration_tag`, `Vehicle_brand`, `VIN_code`, `Vehicle_color`) VALUES
(2, 'О566НР64', 'ГАЗ-3110', '834802358037580', 'бежевый'),
(7, 'О346ТО08', 'Lada Vesta', '2523124454754', 'серебристый'),
(8, 'Н300КО64', 'Audi A6', 'AU23273H3F4232374', 'черный'),
(12, 'Х778РТ777', 'Opel Astra', 'OP4232E3392372L', 'красный'),
(15, 'М131РО36', 'Volkswagen Polo', 'VWV32438238N2376', 'бежевый');

-- --------------------------------------------------------

--
-- Структура таблицы `vehicle`
--

CREATE TABLE IF NOT EXISTS `vehicle` (
  `Vehicle_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Brand_name` varchar(40) DEFAULT NULL,
  `Manufacture_year` varchar(10) DEFAULT NULL,
  `Engine_capacity_hp` int(11) DEFAULT NULL,
  `VIN_code` varchar(30) DEFAULT NULL,
  `Vehicle_colour` varchar(20) DEFAULT NULL,
  `Type_ID` int(11) NOT NULL,
  PRIMARY KEY (`Vehicle_ID`),
  KEY `R_63` (`Type_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=21 ;

--
-- Дамп данных таблицы `vehicle`
--

INSERT INTO `vehicle` (`Vehicle_ID`, `Brand_name`, `Manufacture_year`, `Engine_capacity_hp`, `VIN_code`, `Vehicle_colour`, `Type_ID`) VALUES
(1, 'Volkswagen Passat', '2006', 150, 'WBA4711000781798', 'серебристый', 1),
(2, 'Volkswagen Bora', '2001', 99, 'WVWDB4505LK2345', 'черный', 1),
(3, 'МАЗ 5433', '2004', 360, '55512801002753537', 'белый', 2),
(4, 'ЛиАЗ 5256', '1987', 195, '523423336374647', 'красный', 3),
(5, 'Mazda 6', '2015', 148, 'JMZGD16E2011500', 'атлантика', 1),
(6, 'Лада Приора', '2015', 98, '5335349232825', 'баклажан', 1),
(7, 'Lexus IS300', '2016', 170, 'JN128145195235', 'лазурно-синий', 1),
(8, 'MAN TGX', '2014', 480, 'M1286145N1252', 'красный', 2),
(9, 'Scania R-Series', '2015', 450, 'SC364145367452', 'опатия', 2),
(10, 'Mercedes-Benz GL500', '2016', 240, 'MB35758564322', 'черный', 1),
(11, 'Volvo S60', '2008', 170, 'VV42253647544221', 'жемчуг', 1),
(12, 'КамАЗ 4308', '2008', 280, '42391463719536', 'оранжевый', 2),
(13, 'ЗИЛ 131', '1984', 160, '89185204722723', 'зеленый', 2),
(14, 'Iveco Daily', '2014', 146, 'IC347327H87666F68', 'белый', 3),
(15, 'ПАЗ 320540', '2006', 125, '34636288968424363', 'кристалл', 3),
(16, 'Lada Granta', '2014', 90, '6423565456332791', 'корица', 1),
(17, 'Opel Antara', '2009', 230, 'OP3423E9348712L', 'мираж', 1),
(18, 'Toyota Camry', '2006', 165, 'T25344678963OY39', 'черный', 1),
(19, 'Nissan Juke', '2011', 130, 'NI1353829371S2373', 'белый', 1),
(20, 'Mitsubishi Lancer', '2006', 115, 'M323I9823T263S99', 'лазурит', 1);

-- --------------------------------------------------------

--
-- Структура таблицы `vehicle_hijacking`
--

CREATE TABLE IF NOT EXISTS `vehicle_hijacking` (
  `Hijacking_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Hijacking_date` date DEFAULT NULL,
  `Hijacking_way_ID` int(11) NOT NULL,
  `Status_ID` int(11) NOT NULL,
  `Vehicle_ID` int(11) NOT NULL,
  `Place_ID` int(11) NOT NULL,
  `Status_date` date DEFAULT NULL,
  PRIMARY KEY (`Hijacking_ID`),
  KEY `R_5` (`Hijacking_way_ID`),
  KEY `R_6` (`Status_ID`),
  KEY `R_49` (`Vehicle_ID`),
  KEY `R_50` (`Place_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=8 ;

--
-- Дамп данных таблицы `vehicle_hijacking`
--

INSERT INTO `vehicle_hijacking` (`Hijacking_ID`, `Hijacking_date`, `Hijacking_way_ID`, `Status_ID`, `Vehicle_ID`, `Place_ID`, `Status_date`) VALUES
(1, '2016-04-09', 2, 1, 10, 3, '2016-11-16'),
(2, '2015-10-02', 1, 2, 11, 9, '2016-11-16'),
(3, '2016-05-18', 4, 1, 6, 1, '2016-11-16'),
(4, '2016-10-29', 2, 2, 5, 2, '2016-11-16'),
(5, '2016-06-01', 1, 1, 2, 21, '2016-12-07'),
(7, '2016-12-01', 3, 1, 5, 13, '2016-12-07');

--
-- Триггеры `vehicle_hijacking`
--
DROP TRIGGER IF EXISTS `hj_status`;
DELIMITER //
CREATE TRIGGER `hj_status` BEFORE INSERT ON `vehicle_hijacking`
 FOR EACH ROW BEGIN
  UPDATE `vehicle_registration` SET `hj_status` = 1 WHERE NEW.`vehicle_id` = `vehicle_registration`.`vehicle_id`; 
END
//
DELIMITER ;

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `vehicle_in_accident`
--
CREATE TABLE IF NOT EXISTS `vehicle_in_accident` (
`Регистрационный номер` varchar(10)
,`Марка ТС` varchar(40)
,`ID ТС` int(11)
,`ID участника ДТП` int(11)
);
-- --------------------------------------------------------

--
-- Структура таблицы `vehicle_owner`
--

CREATE TABLE IF NOT EXISTS `vehicle_owner` (
  `Owner_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Phone_number` varchar(20) DEFAULT NULL,
  `Owner_category_ID` int(11) NOT NULL,
  PRIMARY KEY (`Owner_ID`),
  KEY `R_64` (`Owner_category_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=16 ;

--
-- Дамп данных таблицы `vehicle_owner`
--

INSERT INTO `vehicle_owner` (`Owner_ID`, `Phone_number`, `Owner_category_ID`) VALUES
(1, '89063473210', 1),
(2, '593238', 2),
(3, '89607813534', 1),
(4, '673285', 2),
(5, '89553974466', 1),
(6, '89022083511', 1),
(7, '89852787800', 1),
(8, '89352678831', 2),
(9, '89506783472', 1),
(10, '89055609713', 1),
(11, '783290', 2),
(12, '89063452975', 1),
(13, '437500', 2),
(14, '89624774512', 1),
(15, '557733', 2);

-- --------------------------------------------------------

--
-- Структура таблицы `vehicle_registration`
--

CREATE TABLE IF NOT EXISTS `vehicle_registration` (
  `Registration_date` date DEFAULT NULL,
  `Removal_date` date DEFAULT NULL,
  `Registration_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Owner_ID` int(11) NOT NULL,
  `Vehicle_ID` int(11) NOT NULL,
  `Tag_ID` int(11) NOT NULL,
  `hj_status` tinyint(1) DEFAULT '0',
  `accident_status` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`Registration_ID`),
  KEY `R_47` (`Owner_ID`),
  KEY `R_48` (`Vehicle_ID`),
  KEY `R_55` (`Tag_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=22 ;

--
-- Дамп данных таблицы `vehicle_registration`
--

INSERT INTO `vehicle_registration` (`Registration_date`, `Removal_date`, `Registration_ID`, `Owner_ID`, `Vehicle_ID`, `Tag_ID`, `hj_status`, `accident_status`) VALUES
('2015-07-14', NULL, 1, 1, 5, 5, 1, 0),
('2016-03-02', NULL, 2, 8, 3, 9, 0, 0),
('2013-11-09', NULL, 3, 2, 10, 2, 0, 0),
('2016-07-27', NULL, 4, 5, 7, 6, 0, 0),
('2015-12-14', NULL, 5, 9, 6, 7, 0, 0),
('2010-03-15', '2014-10-08', 6, 8, 3, 8, 0, 0),
('2013-10-07', NULL, 7, 11, 13, 14, 0, 0),
('2015-10-14', '2016-09-03', 8, 14, 11, 12, 0, 0),
('2014-02-27', NULL, 9, 13, 14, 15, 0, 0),
('2015-10-30', NULL, 10, 12, 1, 4, 0, 0),
('2012-06-22', NULL, 11, 15, 15, 1, 0, 0),
('2016-06-15', NULL, 12, 3, 9, 11, 0, 0),
('2015-08-19', NULL, 13, 6, 4, 13, 0, 1),
('2013-03-29', '2015-12-16', 14, 10, 12, 3, 0, 0),
('2014-03-09', NULL, 15, 4, 8, 7, 0, 0);

--
-- Триггеры `vehicle_registration`
--
DROP TRIGGER IF EXISTS `hj_vehicle`;
DELIMITER //
CREATE TRIGGER `hj_vehicle` BEFORE INSERT ON `vehicle_registration`
 FOR EACH ROW BEGIN
  IF EXISTS (SELECT `h`.`vehicle_id` FROM `vehicle_hijacking` AS `h` WHERE `h`.`vehicle_id` = NEW.`vehicle_id` AND `h`.`status_id` IN (1, 3)) THEN
    SIGNAL SQLSTATE '45000'
    SET MESSAGE_TEXT = 'This vehicle cannot be added due to its hijacking';
    END IF;
END
//
DELIMITER ;

-- --------------------------------------------------------

--
-- Структура таблицы `vehicle_registration_tag`
--

CREATE TABLE IF NOT EXISTS `vehicle_registration_tag` (
  `Vehicle_registration_tag` varchar(10) DEFAULT NULL,
  `Sort_of_number_ID` int(11) NOT NULL,
  `Tag_ID` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`Tag_ID`),
  KEY `R_1` (`Sort_of_number_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=21 ;

--
-- Дамп данных таблицы `vehicle_registration_tag`
--

INSERT INTO `vehicle_registration_tag` (`Vehicle_registration_tag`, `Sort_of_number_ID`, `Tag_ID`) VALUES
('О788ТО', 6, 1),
('Р007ВС', 5, 2),
('А543МР', 1, 3),
('К767ОС', 6, 4),
('О300ОТ', 3, 5),
('Р555ВО', 3, 6),
('Н450НН', 4, 7),
('Н710ОО', 6, 8),
('Х328РО', 6, 9),
('О020ОО', 1, 10),
('А590МО', 1, 11),
('К100ХО', 3, 12),
('Р276ВА', 6, 13),
('О180РА', 6, 14),
('С001СР', 5, 15),
('Р109РР', 4, 16),
('У130РС', 6, 17),
('О402СА', 6, 18),
('Р886ОТ', 6, 19),
('Р444ОС', 2, 20);

-- --------------------------------------------------------

--
-- Структура таблицы `vehicle_status`
--

CREATE TABLE IF NOT EXISTS `vehicle_status` (
  `Status_ID` int(11) NOT NULL,
  `Status_name` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`Status_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `vehicle_status`
--

INSERT INTO `vehicle_status` (`Status_ID`, `Status_name`) VALUES
(1, 'найдено'),
(2, 'в поиске'),
(3, 'не раскрыто');

-- --------------------------------------------------------

--
-- Структура таблицы `vehicle_type`
--

CREATE TABLE IF NOT EXISTS `vehicle_type` (
  `Type_ID` int(11) NOT NULL,
  `Type` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`Type_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `vehicle_type`
--

INSERT INTO `vehicle_type` (`Type_ID`, `Type`) VALUES
(1, 'легковой транспорт'),
(2, 'грузовой транспорт'),
(3, 'пассажирский транспорт');

-- --------------------------------------------------------

--
-- Дублирующая структура для представления `view_registered_vehicle`
--
CREATE TABLE IF NOT EXISTS `view_registered_vehicle` (
`ID ТС` int(11)
,`Марка ТС` varchar(40)
,`Цвет` varchar(20)
,`Тип ТС` int(11)
);
-- --------------------------------------------------------

--
-- Дублирующая структура для представления `view_vehicle_with_tag`
--
CREATE TABLE IF NOT EXISTS `view_vehicle_with_tag` (
`Регистрационный номер` varchar(10)
,`Марка ТС` varchar(40)
,`ID ТС` int(11)
);
-- --------------------------------------------------------

--
-- Структура таблицы `way_of_hijacking`
--

CREATE TABLE IF NOT EXISTS `way_of_hijacking` (
  `Hijacking_way_ID` int(11) NOT NULL,
  `Way_name` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`Hijacking_way_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

--
-- Дамп данных таблицы `way_of_hijacking`
--

INSERT INTO `way_of_hijacking` (`Hijacking_way_ID`, `Way_name`) VALUES
(1, 'без взлома'),
(2, 'взлом дверного замка'),
(3, 'перехват радиосигнала'),
(4, 'подключение к CAN-шине');

-- --------------------------------------------------------

--
-- Структура для представления `owners`
--
DROP TABLE IF EXISTS `owners`;

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `owners` AS (select `o`.`Owner_ID` AS `ID Владельца`,`o`.`Phone_number` AS `Телефон`,`o`.`Owner_category_ID` AS `Категория владельца`,`i`.`Full_name` AS `ФИО`,`l`.`Company_name` AS `Название компании` from ((`vehicle_owner` `o` left join `individual` `i` on((`o`.`Owner_ID` = `i`.`Owner_ID`))) left join `legal_entity` `l` on((`o`.`Owner_ID` = `l`.`Owner_ID`))));

-- --------------------------------------------------------

--
-- Структура для представления `stolen_vehicle`
--
DROP TABLE IF EXISTS `stolen_vehicle`;

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `stolen_vehicle` AS (select `h`.`Vehicle_ID` AS `ID ТС`,`v`.`Brand_name` AS `Марка_ТС` from (`vehicle_hijacking` `h` join `vehicle` `v` on((`v`.`Vehicle_ID` = `h`.`Vehicle_ID`))));

-- --------------------------------------------------------

--
-- Структура для представления `vehicle_in_accident`
--
DROP TABLE IF EXISTS `vehicle_in_accident`;

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `vehicle_in_accident` AS (select `t`.`Vehicle_registration_tag` AS `Регистрационный номер`,`v`.`Brand_name` AS `Марка ТС`,`v`.`Vehicle_ID` AS `ID ТС`,`rv`.`Participant_ID` AS `ID участника ДТП` from (((`vehicle_registration_tag` `t` join `vehicle_registration` `r` on((`r`.`Tag_ID` = `t`.`Tag_ID`))) join `vehicle` `v` on((`r`.`Vehicle_ID` = `v`.`Vehicle_ID`))) join `registered_vehicle` `rv` on((`rv`.`Vehicle_ID` = `v`.`Vehicle_ID`))));

-- --------------------------------------------------------

--
-- Структура для представления `view_registered_vehicle`
--
DROP TABLE IF EXISTS `view_registered_vehicle`;

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `view_registered_vehicle` AS (select `r`.`Vehicle_ID` AS `ID ТС`,`v`.`Brand_name` AS `Марка ТС`,`v`.`Vehicle_colour` AS `Цвет`,`v`.`Type_ID` AS `Тип ТС` from (`vehicle` `v` join `vehicle_registration` `r` on((`r`.`Vehicle_ID` = `v`.`Vehicle_ID`))));

-- --------------------------------------------------------

--
-- Структура для представления `view_vehicle_with_tag`
--
DROP TABLE IF EXISTS `view_vehicle_with_tag`;

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `view_vehicle_with_tag` AS (select `t`.`Vehicle_registration_tag` AS `Регистрационный номер`,`v`.`Brand_name` AS `Марка ТС`,`v`.`Vehicle_ID` AS `ID ТС` from ((`vehicle_registration_tag` `t` join `vehicle_registration` `r`) join `vehicle` `v`) where ((`r`.`Tag_ID` = `t`.`Tag_ID`) and (`v`.`Vehicle_ID` = `r`.`Vehicle_ID`)));

--
-- Ограничения внешнего ключа сохраненных таблиц
--

--
-- Ограничения внешнего ключа таблицы `car_transport`
--
ALTER TABLE `car_transport`
  ADD CONSTRAINT `car_transport_ibfk_1` FOREIGN KEY (`Vehicle_ID`) REFERENCES `vehicle` (`Vehicle_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `freight_transport`
--
ALTER TABLE `freight_transport`
  ADD CONSTRAINT `freight_transport_ibfk_1` FOREIGN KEY (`Vehicle_ID`) REFERENCES `vehicle` (`Vehicle_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `individual`
--
ALTER TABLE `individual`
  ADD CONSTRAINT `individual_ibfk_1` FOREIGN KEY (`Owner_ID`) REFERENCES `vehicle_owner` (`Owner_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `legal_entity`
--
ALTER TABLE `legal_entity`
  ADD CONSTRAINT `legal_entity_ibfk_1` FOREIGN KEY (`Owner_ID`) REFERENCES `vehicle_owner` (`Owner_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `participant_of_accident`
--
ALTER TABLE `participant_of_accident`
  ADD CONSTRAINT `participant_of_accident_ibfk_1` FOREIGN KEY (`Participant_category_ID`) REFERENCES `participant_category` (`Participant_category_ID`);

--
-- Ограничения внешнего ключа таблицы `participation_in_accident`
--
ALTER TABLE `participation_in_accident`
  ADD CONSTRAINT `participation_in_accident_ibfk_1` FOREIGN KEY (`Extent_ID`) REFERENCES `extent_of_participation` (`Extent_ID`),
  ADD CONSTRAINT `participation_in_accident_ibfk_2` FOREIGN KEY (`Accident_ID`) REFERENCES `road_accident` (`Accident_ID`),
  ADD CONSTRAINT `participation_in_accident_ibfk_3` FOREIGN KEY (`Participant_ID`) REFERENCES `participant_of_accident` (`Participant_ID`);

--
-- Ограничения внешнего ключа таблицы `passenger_transport`
--
ALTER TABLE `passenger_transport`
  ADD CONSTRAINT `passenger_transport_ibfk_1` FOREIGN KEY (`Vehicle_ID`) REFERENCES `vehicle` (`Vehicle_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `pedestrian`
--
ALTER TABLE `pedestrian`
  ADD CONSTRAINT `pedestrian_ibfk_1` FOREIGN KEY (`Participant_ID`) REFERENCES `participant_of_accident` (`Participant_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `place_of_accident`
--
ALTER TABLE `place_of_accident`
  ADD CONSTRAINT `place_of_accident_ibfk_1` FOREIGN KEY (`Place_category_ID`) REFERENCES `place_category` (`Place_category_ID`);

--
-- Ограничения внешнего ключа таблицы `registered_vehicle`
--
ALTER TABLE `registered_vehicle`
  ADD CONSTRAINT `registered_vehicle_ibfk_1` FOREIGN KEY (`Participant_ID`) REFERENCES `participant_of_accident` (`Participant_ID`) ON DELETE CASCADE,
  ADD CONSTRAINT `registered_vehicle_ibfk_2` FOREIGN KEY (`Vehicle_ID`) REFERENCES `vehicle` (`Vehicle_ID`);

--
-- Ограничения внешнего ключа таблицы `road_accident`
--
ALTER TABLE `road_accident`
  ADD CONSTRAINT `road_accident_ibfk_1` FOREIGN KEY (`Type_of_accident_ID`) REFERENCES `type_of_accident` (`Type_of_accident_ID`),
  ADD CONSTRAINT `road_accident_ibfk_2` FOREIGN KEY (`Place_ID`) REFERENCES `place_of_accident` (`Place_ID`);

--
-- Ограничения внешнего ключа таблицы `section_of_the_route`
--
ALTER TABLE `section_of_the_route`
  ADD CONSTRAINT `section_of_the_route_ibfk_1` FOREIGN KEY (`Place_ID`) REFERENCES `place_of_accident` (`Place_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `settlement`
--
ALTER TABLE `settlement`
  ADD CONSTRAINT `settlement_ibfk_1` FOREIGN KEY (`Place_ID`) REFERENCES `place_of_accident` (`Place_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `unregistered_vehicle`
--
ALTER TABLE `unregistered_vehicle`
  ADD CONSTRAINT `unregistered_vehicle_ibfk_1` FOREIGN KEY (`Participant_ID`) REFERENCES `participant_of_accident` (`Participant_ID`) ON DELETE CASCADE;

--
-- Ограничения внешнего ключа таблицы `vehicle`
--
ALTER TABLE `vehicle`
  ADD CONSTRAINT `vehicle_ibfk_1` FOREIGN KEY (`Type_ID`) REFERENCES `vehicle_type` (`Type_ID`);

--
-- Ограничения внешнего ключа таблицы `vehicle_hijacking`
--
ALTER TABLE `vehicle_hijacking`
  ADD CONSTRAINT `vehicle_hijacking_ibfk_1` FOREIGN KEY (`Hijacking_way_ID`) REFERENCES `way_of_hijacking` (`Hijacking_way_ID`),
  ADD CONSTRAINT `vehicle_hijacking_ibfk_2` FOREIGN KEY (`Status_ID`) REFERENCES `vehicle_status` (`Status_ID`),
  ADD CONSTRAINT `vehicle_hijacking_ibfk_3` FOREIGN KEY (`Vehicle_ID`) REFERENCES `vehicle` (`Vehicle_ID`),
  ADD CONSTRAINT `vehicle_hijacking_ibfk_4` FOREIGN KEY (`Place_ID`) REFERENCES `place_of_accident` (`Place_ID`);

--
-- Ограничения внешнего ключа таблицы `vehicle_owner`
--
ALTER TABLE `vehicle_owner`
  ADD CONSTRAINT `vehicle_owner_ibfk_1` FOREIGN KEY (`Owner_category_ID`) REFERENCES `owner_category` (`Owner_category_ID`);

--
-- Ограничения внешнего ключа таблицы `vehicle_registration`
--
ALTER TABLE `vehicle_registration`
  ADD CONSTRAINT `vehicle_registration_ibfk_1` FOREIGN KEY (`Owner_ID`) REFERENCES `vehicle_owner` (`Owner_ID`),
  ADD CONSTRAINT `vehicle_registration_ibfk_2` FOREIGN KEY (`Vehicle_ID`) REFERENCES `vehicle` (`Vehicle_ID`),
  ADD CONSTRAINT `vehicle_registration_ibfk_3` FOREIGN KEY (`Tag_ID`) REFERENCES `vehicle_registration_tag` (`Tag_ID`);

--
-- Ограничения внешнего ключа таблицы `vehicle_registration_tag`
--
ALTER TABLE `vehicle_registration_tag`
  ADD CONSTRAINT `vehicle_registration_tag_ibfk_1` FOREIGN KEY (`Sort_of_number_ID`) REFERENCES `sort_of_number` (`Sort_of_number_ID`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
