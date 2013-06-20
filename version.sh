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
sed -i.old -e 's/^pkgrel=.*$/pkgrel=1/' PKGBUILD
cd ../..


#sourcecode version.h
sed -i.old -e 's/^#define VERSION .*$/#define VERSION "v'$ver'"/' version.h



