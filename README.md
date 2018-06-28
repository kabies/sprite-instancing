# sprite-instancing

OpenGL の Instancing を使ったスプライト描画の高速化を検証したコード

WebGL 1.0 で動作させる場合、ビルドに emscripten が必要

macOS でデスクトップ OpenGL 2.1 を使って動作させる場合、 SDL2 と glew が必要。
SDL2は `SDL2.framework` と `SDL2_image.framework` がホームディレクトリの `Library/Frameworks` にある想定、
glewはmacportsでインストールしてある想定、となってるので、なんか違う場合 `build-macos.sh` を適当に変える。

# 解説

デフォルトだと640スプライトが整列した状態で表示される。
コードいじって毎フレームランダムな位置に表示させたり、スプライト数を増やしたりする場合、
負荷が高かったり目に悪かったりするので注意。

参考程度に、32,768スプライト@MacBook Air 11inch で 122FPS、 WebGL版が Firefox 61.0 で30FPSだった。

このバージョンではシェーダー内でスプライトの座標変換を行なっている。
シーングラフ的なシステムを組むなら、これはC側に持って来る必要がある。
その場合、 `vec4` を四つ使ってインスタンス毎の変換行列をシェーダーへ送り、
シェーダー内で座標変換用の `mat4` を作って掛け算して…という感じになる。

texture_zとしてテクスチャの指定を渡しているのは、当初テクスチャ配列を使おうと思っていた名残。
OpenGL ES 2.0 / WebGL 1.0 には（たぶん拡張機能ふくめても）テクスチャ配列はなかった。

# スクショ

![整列](ss_a.jpg)

![ランダム配置](ss_b.jpg)
