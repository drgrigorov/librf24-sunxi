#!/bin/bash
rsync -racv --exclude-from=.gitignore src/ micro:librf24-sunxi/src && rsync -racv --exclude-from=.gitignore test/ micro:librf24-sunxi/test
