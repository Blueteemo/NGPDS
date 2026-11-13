# NGPDS — Fusion Edition

[English](README.md) · [简体中文](README.zh-CN.md) · **日本語**

**[v1.0.0 をダウンロード（.nds）](https://github.com/Blueteemo/NGPDS/releases/download/v1.0.0/NGPDS_Fusion_v1.0.0.nds)** · [リリースノート](https://github.com/Blueteemo/NGPDS/releases/tag/v1.0.0) · [ソースと依存ライブラリ（ZIP）](https://github.com/Blueteemo/NGPDS/releases/download/v1.0.0/NGPDS_Fusion_v1.0.0_Source.zip) · [SHA-256](https://github.com/Blueteemo/NGPDS/releases/download/v1.0.0/SHA256SUMS.txt)

このブランチには以前の統合版のソースが含まれます。v1.0.0 のソースとビルドに必要な依存ライブラリは、上の ZIP に収録されています。

Fusion Edition は、**Fredrik Ahlström（FluBBaOfWard）** が開発した Nintendo DS 系向けネオジオポケット／ネオジオポケットカラーエミュレーター NGPDS の非公式フォークです。**Blueteemo** がメンテナンスしています。

[オリジナルプロジェクト](https://github.com/FluBBaOfWard/NGPDS) · [オリジナルの説明書](Docs/UPSTREAM_README.md)

v1.0.0 のバージョン表示は `V1.0.0 2026-11-13` です。

## 主な変更

- 『Memories Off Pure』の立ち絵切り替え、背景スクロール、プロフィール表示を改善し、『INFINITY cure.』で確認した本文の表示不具合を修正しました。
- フラッシュセーブと設定の自動保存、書き込み時の検証、バックアップからの復旧に対応しています。
- 英語・簡体字中国語・日本語のメニュー、画面枠の切り替え、彩度設定を追加しました。
- **オプション → 一般 → 音声モード**で、標準（初期設定）とクラシック（従来のミキサー）を切り替えられます。起動時に短いフェードインをかけます。以前の NDSi＋R4 での確認では、起動時のポップノイズは発生しませんでした。

## 使い方

1. `NGPDS_Fusion_v1.0.0.nds` をダウンロードし、カードにコピーします。ROM はご自身で用意してください。対応ソフトのフラッシュセーブには内蔵 HLE BIOS も使えます。更新時はセーブと設定をバックアップしてください。 [ビルド手順](README.md#build)
2. 下画面をタッチするか **L+R** を同時に押してメニューを開きます。表示言語は **オプション → 一般 → 表示言語** で変更できます。
3. 通常はボタンとタッチでメニューを操作します。一時停止をオフにすると **メニュー中のゲーム操作** が表示されます。これをオンにすると、ゲーム起動後はボタンでゲームを、タッチでメニューを操作します。
4. **メニュー外で消灯** をオンにすると、メニューを閉じたときにゲームを表示していない画面が消灯します。フラッシュセーブの自動保存をオフにしている場合は、ファイルメニューから手動で保存してください。

## 確認範囲とクレジット

以前のビルドでメニューと起動音を NDSi＋R4 で確認しました。melonDS では『INFINITY cure.』日本語版を内蔵 HLE BIOS で起動し、ゲーム内でのセーブと再起動後の再開を確認しました。v1.0.0 のソース ZIP から再ビルドしたファイルは、リリースの実行ファイルとバイト単位で一致しました。R4 へのコピー後もチェックサムが一致しました。 [検証の詳細（英語）](README.md#verification-and-limits)

原作者は **Fredrik Ahlström（FluBBaOfWard）**、フォークのメンテナンスは **Blueteemo** です。オリジナルの著作権表示と各依存ライブラリのライセンスを保持しています。 [Fusion Pixel フォントのクレジットとライセンス](https://github.com/Blueteemo/NGPDS/tree/a3b33264382e5056ab8b997d76957dd1607d0ec8/third_party/fusion-pixel-12px/)

## 過去のソースアーカイブ

[v0.2.3 のソースアーカイブ](https://github.com/Blueteemo/NGPDS/raw/d5389c337dc0f4e93cac3e75bf18e2be0ffd602b/NGPDS_Fusion_v0.2.3_Source.zip) · [SHA-256](https://github.com/Blueteemo/NGPDS/blob/d5389c337dc0f4e93cac3e75bf18e2be0ffd602b/SHA256SUMS.txt)
