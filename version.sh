#!/bin/bash

#Global vars
ver=$1
datever=$(date +%Y%m%d)_$1

echo ver=$ver
echo datever=$datever


#VERSION.txt

echo $1 > VERSION.txt


#Archlinux PKGBUILD

cd linux/archlinux/
sed -i.old -e 's/^pkgver=.*$/pkgver='$datever'/' PKGBUILD
cd ../..


#sourcecode version.h
sed -i.old -e 's/^#define VERSION .*$/#define VERSION '$ver'/' version.h



