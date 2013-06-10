/*******************************************
 *
 * Copyright © 2013 Léo Flaventin Hauchecorne
 *
 * This file is part of Ardoise.
 *
 * Ardoise is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ardoise is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************/

#error this file shuold not be included, it's just to make lupdate to considerate some strings that are not directely put in the sources

#include <QtGlobal>

constexpr const struct { const char* const source, comment; } * const stringToTranslate = {

QT_TRANSLATE_NOOP3("OptionsWidget", "general", "Onglet général du widget de configuration")

};
