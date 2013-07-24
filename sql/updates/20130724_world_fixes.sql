UPDATE `version` SET `db_version` = 'BDB_20130724';

-- Nightbane Fix
UPDATE `creature_template` SET `inhabitType` = '4' WHERE `entry` = '17225';
-- Boss Shade of Aran Fix
UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = '18254';