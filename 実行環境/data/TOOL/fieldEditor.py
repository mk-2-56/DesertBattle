import maya.cmds as cmds
import struct
from PySide2 import QtWidgets, QtGui, QtCore
from enum import IntEnum
#================================================================
# マクロ定義
#================================================================
ATTR_NUM     = 3
WINDOW_NAME  = "FieldEditor"
WINDOW_TITLE = "Field Editor ver.1.10"
NAME_MESH = "name_mesh"
NUM_BLOCK = "num_block"
SIZE_BLOCK = "size_block"
DROPDOWN_OBJ = "DropdownObj"
TEXT_OUTPUT  = "TextOutput"
class ATTR(IntEnum):
    TX = 0
    TY = 1
    TZ = 2


#================================================================
# グローバル変数
#================================================================
# 頂点データ格納用
nodes = []
# 取得アトリビュート
attrs = [
        "tx", "ty", "tz"
        ]
# 選択しているオブジェクト名
selectedObj = ""
fileName    = ""
meshName    = NAME_MESH
numBlock    = NUM_BLOCK
sizeBlock   = SIZE_BLOCK


#================================================================
# 関数
#================================================================
#----------------------------------------------------------------
# 現在のシーンのディレクトリの取得
#----------------------------------------------------------------
def GetCurrentDir(self):
    # 現在のシーンのファイルパスを取得
    scenePath = cmds.file(query=True, sceneName=True)
    pathList = scenePath.split("/")
    sceneName = pathList[-1]
    
    if scenePath:
        # ファイルパスからディレクトリを抽出
        sceneDir = scenePath.replace(sceneName, '')
        return sceneDir
    else:
        return None
        
#----------------------------------------------------------------
# フィールドのブロック数を取得
#----------------------------------------------------------------
def GetNumBlock(self):
    textValue = cmds.intField(NUM_BLOCK, query=True, value=True)
    #print("テキストボックスの値:", text_value)
    global numBlock
    numBlock = textValue
    #print(fileName)
    print("フィールドのブロック数を更新しました -> {}".format(numBlock))

#----------------------------------------------------------------
# フィールドのサイズを取得
#----------------------------------------------------------------
def GetSizeBlock(self):
    textValue = cmds.floatField(SIZE_BLOCK, query=True, value=True)
    #print("テキストボックスの値:", text_value)
    global sizeBlock
    sizeBlock = textValue
    #print(fileName)
    print("フィールドのサイズを更新しました -> {}".format(sizeBlock))

#----------------------------------------------------------------
# テキストボックスの値を取得(メッシュ名)
#----------------------------------------------------------------
def GetNameMesh(self):
    textValue = cmds.textField(NAME_MESH, query=True, text=True)
    #print("テキストボックスの値:", text_value)
    global meshName
    meshName = textValue
    #print(fileName)
    print("メッシュ名を更新しました -> {}".format(meshName))
    UpdateDropdownList(self)
    
#----------------------------------------------------------------
# テキストボックスの値を取得(出力ファイル名)
#----------------------------------------------------------------
def GetTextBoxOutput(self):
    textValue = cmds.textField(TEXT_OUTPUT, query=True, text=True)
    #print("テキストボックスの値:", text_value)
    global fileName
    fileName = textValue
    #print(fileName)
    print("出力ファイル名を更新しました -> {}".format(fileName))

#----------------------------------------------------------------
# ドロップダウンリストの値を取得
#----------------------------------------------------------------
def GetDropdownListObj(self):
    selectedItem = cmds.optionMenu(DROPDOWN_OBJ, query=True, value=True)
    #print("選択されたアイテム:", selected_item)
    global selectedObj
    selectedObj = selectedItem
    #print(selectedObj)
    GetVertexData(self)
#----------------------------------------------------------------
# ドロップダウンリストのアイテムを更新
#----------------------------------------------------------------
def UpdateDropdownList(self):
    # ドロップダウンリストのアイテムをリセット
    cmds.optionMenu(DROPDOWN_OBJ, edit=True, deleteAllItems=True)
    # ドロップダウンリストにアイテムを追加
    sceneObjs = cmds.ls(meshName+"*", transforms=True)
    for sObj in sceneObjs:
        cmds.menuItem(label=sObj, parent=DROPDOWN_OBJ)
    if len(sceneObjs) > 0:
        global selectedObj
        selectedObj = sceneObjs[0]

#----------------------------------------------------------------
# パターンを計算
#----------------------------------------------------------------
def CalculatePattern(num_vertices):
    if int(num_vertices**0.5)**2 != num_vertices:
        print("指定された数字は二乗数ではありません。")
        return None

    pattern = []
    sqrt_num = int(num_vertices**0.5)

    for i in range(sqrt_num):
        for j in range(sqrt_num):
            value = i + j * sqrt_num
            if value not in pattern:
                pattern.append(value)

    return pattern

#----------------------------------------------------------------
# 頂点情報を取得
#----------------------------------------------------------------
def GetVertexData(self):
     # オブジェクトの総頂点数を取得
    vertex_count = cmds.polyEvaluate(selectedObj, vertex=True)
    # 結果を表示
    print("選択したオブジェクトの総頂点数: {}".format(vertex_count))
    num_vertices = vertex_count

    # リストの要素をリセットする
    nodes.clear()

    # パターンを計算
    pattern = CalculatePattern(num_vertices)

    if pattern is not None:
         # オブジェクトの頂点を取得
        strings = cmds.polyListComponentConversion(selectedObj, toVertex=True)
        vertices = cmds.filterExpand(strings, selectionMask=31)

        # パターンに従って頂点の座標を取得して出力
        for p in pattern:
            vertex_position = cmds.pointPosition(vertices[p], world=True)
            formatted_position = ["{:.1f}".format(round(coord * (sizeBlock * 10), 1)) for coord in vertex_position]
            nodes.extend(formatted_position)
    
    else:
        for p in pattern:
            nodes.extend([0.0, 0.0])
        print("頂点データが見つかりませんでした -> 空データで更新しました")

#----------------------------------------------------------------
# メッシュフィールドの作成
#----------------------------------------------------------------
def createField(self):
    # 頂点数に基づいてPlaneを作成
    plane = cmds.polyPlane(w=1, h=1, sx=numBlock, sy=numBlock, name='customPlane')[0]
    
#----------------------------------------------------------------
# リストの要素をCSVファイルに書き込む
#----------------------------------------------------------------
def WriteCSV(self):
    # 現在のシーンディレクトリを取得する
    currentDir = GetCurrentDir(self)
    # ファイル名が入力されていない場合、処理を中断
    global fileName
    if fileName == "":
        print("ファイル名を入力してください")
        return
    # キーフレームデータがない場合、処理を中断
    if len(nodes) == 0:
        print("データが見つかりませんでした")
        return
    # 書き込むファイル名の拡張子をテキストファイルにする
    outputName = fileName + ".csv"
    # ファイルパスを構築
    filePath = currentDir + outputName
    # ファイルを書き込みモードで開く
    with open(filePath, "w") as file:
        # リストの要素を書き込んでいく
        file.write("tx,ty,tz\n")
        for i in range(0, len(nodes), ATTR_NUM):
            file.write("{},{},{}\n".format( 
                nodes[i],
                nodes[i + 1],
                nodes[i + 2],
           ))
        print("書き込みに成功しました -> {}".format(outputName))
        
#================================================================
# ウィンドウ
#================================================================
# ウィンドウの作成
windowName = WINDOW_NAME
if cmds.window(windowName, exists=True):
    cmds.deleteUI(windowName, window=True)
    
cmds.window(windowName, title=WINDOW_TITLE, widthHeight=[300, 600])

#================================================================
# UI
#================================================================
# UI要素の追加
#cmds.columnLayout(adjustableColumn=True)
cmds.columnLayout(columnAttach=["both",5], adjustableColumn=True)
cmds.text(label="\nフィールドエディター\n", height=50, backgroundColor=[1.0,1.0,1.0], font="boldLabelFont")

#----------------------------------------------------------------
# テキストボックスの作成(フィールドのブロック数)
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=2, height=50, 
               columnWidth2=(110, 110), 
               columnAttach=([1,"both",0]))
cmds.text(label="ブロック数:", font="boldLabelFont")
cmds.intField(NUM_BLOCK, 
               width=150, height=30, minValue=0,
               changeCommand=GetNumBlock)
cmds.setParent('..')

#----------------------------------------------------------------
# ボタンの作成 ※フィールドを作る
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=2, 
               height=50, 
               columnWidth2=(110, 110), 
               columnAttach=([1,"both",0]))
# ボタンの追加
#button1 = cmds.button(label="データ更新", command=GetKeyframeData)
cmds.text(label="フィールドの作成:", font="boldLabelFont")
button1 = cmds.button(label="　　　作成！　　　", command=createField)
# レイアウトの終了
cmds.setParent('..')

#----------------------------------------------------------------
# テキストボックスの作成(メッシュの検索)
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=1, height=50, columnAttach=[1,"both",75])
cmds.textField(NAME_MESH, 
               width=150, height=30, 
               placeholderText="ここにメッシュ名を入力", 
               changeCommand=GetNameMesh)
cmds.setParent('..')

#----------------------------------------------------------------
# テキストボックスの作成(フィールドのサイズ)
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=2, height=50, 
               columnWidth2=(110, 110), 
               columnAttach=([1,"both",0]))
cmds.text(label="フィールドのサイズ:", font="boldLabelFont")
cmds.floatField(SIZE_BLOCK, 
               width=150, height=30, minValue=0,
               changeCommand=GetSizeBlock)
cmds.setParent('..')

#----------------------------------------------------------------
# ドロップダウンリストの作成
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=1, height=50, columnAttach=[1,"both",20])
cmds.optionMenu(DROPDOWN_OBJ, 
                label="メッシュを選択してください",
                changeCommand=GetDropdownListObj)
cmds.setParent('..')

#----------------------------------------------------------------
# テキストボックスの作成(出力ファイル名)
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=1, height=50, columnAttach=[1,"both",75])
cmds.textField(TEXT_OUTPUT, 
               width=150, height=30, 
               placeholderText="ここに出力ファイル名を入力", 
               changeCommand=GetTextBoxOutput)
cmds.setParent('..')

#----------------------------------------------------------------
# ボタンの作成 ※更新
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=2, 
               height=50, 
               columnWidth2=(110, 110), 
               columnAttach=([1,"both",0]))
# ボタンの追加
#button1 = cmds.button(label="データ更新", command=GetKeyframeData)
cmds.text(label="出力前に必ず\n頂点情報を更新してね:", font="boldLabelFont")
button2 = cmds.button(label="　　　更新！　　　", command=GetVertexData)
# レイアウトの終了
cmds.setParent('..')

#----------------------------------------------------------------
# ボタンの作成 ※単体出力
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=2, 
               height=50, 
               columnWidth2=(110, 110), 
               columnAttach=([1,"both",0]))
# ボタンの追加
#button1 = cmds.button(label="データ更新", command=GetKeyframeData)
cmds.text(label="csvの出力:", font="boldLabelFont")
button3 = cmds.button(label="　　　出力！　　　", command=WriteCSV)
# レイアウトの終了
cmds.setParent('..')

# ウィンドウの表示
cmds.showWindow(windowName)