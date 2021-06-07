# SPDX-License-Identifier: Apache-2.0

# Obrigatorio: o Zephyr exige cmake minimo para funcionar
cmake_minimum_required(VERSION 3.13.1)

# Obrigatório: O cmake precisa achar o Zephyr
find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})

# Obrigatorio: mas o nome fica do seu gosto:
project(esp32_wifi_station)

# Obrigatorio: adicione aqui seus arquivos C para o simbolo de sources
FILE(GLOB app_sources src/*.c)

# Obrigatorio: em seguida alimente o simbolo "app" com os arquivos adicionados
target_sources(app PRIVATE ${app_sources})
