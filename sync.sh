#!/bin/bash
rsync -racv src/ shtaigalan:librf24-sunxi/src && rsync -racv test/ shtaigalan:librf24-sunxi/test
