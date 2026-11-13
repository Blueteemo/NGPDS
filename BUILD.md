# NGPDS Fusion Edition v0.2.3 source archive

The eight Git bundles contain NGPDS and its seven pinned dependencies,
including the modified SN76496 code and the nested Unzip dependency.
The NGPDS bundle contains the final banner icon and graphite border.

The verified toolchain is Wonderful arm-none-eabi GCC 16.2.0 with
BlocksDS 1.24.0. Install the toolchain separately. With Git, make and
shasum available, run these commands from the extracted archive:

```sh
shasum -a 256 -c BUNDLE_SHA256SUMS.txt
export WONDERFUL_TOOLCHAIN=/path/to/wonderful
sh rebuild.sh /path/to/new-checkout
```

Choose a checkout directory that does not already exist. The script
restores the source and dependencies offline, builds `NGPDS.nds` in
that directory and verifies its SHA-256. Set `BLOCKSDS` explicitly if
your BlocksDS installation is outside the default Wonderful directory.

The expected build hash in `EXPECTED_NDS_SHA256.txt` matches the v0.2.3
binary installed on R4. Source commit and dependency pins are recorded
in `DEPENDENCIES.txt`. Credits and license notices are included in
the source and dependency bundles.

## 简体中文

解压后进入本目录，按上方命令校验归档并重建。请将
`WONDERFUL_TOOLCHAIN` 改为已安装的 Wonderful 路径，指定一个尚不存在的检出目录。
脚本从本地 Git bundle 恢复源码及依赖，无需联网，并在编译后校验 `NGPDS.nds`。

## 日本語

展開後、このディレクトリで上記のコマンドを実行してください。
`WONDERFUL_TOOLCHAIN` にインストール済みの Wonderful のパスを指定し、
チェックアウト先には新規ディレクトリを指定します。ソースと依存ライブラリは
同梱の Git bundle から復元され、ビルド後に `NGPDS.nds` の SHA-256 が検証されます。
