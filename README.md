# VSCODE OpenGL 环境配置(Mingw, GLFW, GLAD)

1. 安装 mingw 并配置`.vscode/c_cpp_properties.json`

    > https://code.visualstudio.com/docs/cpp/config-mingw

2. vscode 创建`launch.json`,`tasks.json`文件

    > 1. launch.json:
    >    点击侧边栏**Run and Debug(Ctrl+Shift+D)**, 点击**Create launch.json**
    > 2. tasks.josn:
    >    点击菜单栏**Terminal->Configure Default Build Task...**

3. 修改 launch.json 文件

    > 在根目录下创建**build**目录，用于存放生成的.exe 可执行文件
    > launch.json 修改"program"为:
    >
    > ```json
    > "program": "${workspaceFolder}/build/${fileBasenameNoExtension}.exe",
    > ```

4. 修改 tasks.json 文件
    > 修改"args"为:
    >
    > ```json
    > "args": [
    >            "${file}",
    >            "${workspaceFolder}/src/glad.c",
    >            "-g",
    >            "-o",
    >            "${workspaceFolder}/build/${fileBasenameNoExtension}.exe",
    >            "-ID:/openglrepo/Include",
    >            "-LD:/openglrepo/Libs",
    >            "-lglfw3dll",
    >            "-lopengl32"
    >        ],
    > ```
5. 添加 `glad/src/glad.c` 到 src 目录，添加`glfw3.dll`到 build 目录
