# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]
### Changed
* Remove Icon font from library
* Support pico-sdk 2.0.0

### Changed
* OLED_BLK_Set_PWM(0) leads changing into GPIO mode to set false

## [1.0.1] - 2024-03-31
### Added
* Add LCD_Config()
* Support BLK PWM control

### Changed
* LCD_SetRotation(0) makes relative 0 degree rotation against configuration (previous: absolute rotation of the module)

## [1.0.0] - 2024-03-02
* Initial release