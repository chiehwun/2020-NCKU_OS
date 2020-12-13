# Key-Value Storages

<!-- ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ -->

## 基本資料

| 學號      | 姓名   | 系級     |
| --------- | ------ | -------- |
| E14066282 | 溫梓傑 | 機械 110 |

---

:::info

## 開發環境

- **OS:** Ubuntu 18.04.4 LTS
- **CPU:** Intel® Core™ i5-6300HQ CPU @ 2.30GHz × 4
- **Memory:** 7.7 GiB
- **Swap:** 1.37GiB
- **Language:** C++ (standard 11)
- **Compiler:** g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
  :::

---

## 程式架構簡介

本程式是以 B+ Tree 為基礎，所建構的 Key-value storage 程式，使用 B+ Tree 的好處是，所有資料節點都在樹的最底層，而且 B+ Tree 在插入、刪除的時間皆為 O(log n)，非常適合作為資料庫程式的基礎。

### PUT 指令

![](https://i.imgur.com/DIjvOBH.png)

### GET 指令

![](https://i.imgur.com/6j7JM98.png)

### B+ Tree insertion

程式碼參考自
https://www.geeksforgeeks.org/insertion-in-a-b-tree/
P.S. 其實那個網站的程式是有**很多**BUG 的
![](https://i.imgur.com/XtpF3NZ.png)

### LRU algorithm implementation

程式碼參考自
https://www.geeksforgeeks.org/lru-cache-implementation/

<!-- ![](https://i.imgur.com/DWvBdYL.png) -->

## 程式執行時間

| PUT instr. num | Execution time | \*.file number |
| :------------- | -------------- | -------------- |
| 10             | 258 ms         | 1              |
| 100            | 1,188 ms       | 1              |
| 1,000          | 18,469 ms      | 1              |
| 10,000         | 68,453 ms      | 1              |
| 100,000        | 512,485 ms     | 1              |
| 1,000,000      | 5,023,911 ms   | 2              |
| 10,000,000     | 57,848,985 ms  | 5              |

![](https://i.imgur.com/NCc2pPp.png)
::: success
**[圖一]CPU time v.s. PUT instruction number**
可以發現在 thread number = 2 時，CPU time 最小，並且在[圖一]中，大致上呈現線性分佈。
:::

## 程式開發與使用說明

### How to run code

```console=1
# Compile
$ make
# Run
$ ./hw3.out [./*.input_file]
```

---

## 效能分析報告

#### 程式執行期間各個 Stage 對電腦資源使用情形

![](https://i.imgur.com/soPouNd.jpg)
![](https://i.imgur.com/mqaOILA.png)

:::warning
**[圖三]、[圖四] 不同階段使用硬體資源之狀況**

| Stage          | CPU        | Memory         | I/O        |
| :------------- | ---------- | -------------- | ---------- |
| Start 程式開始 | high usage | 漸漸升高       | none       |
| A. Dump File   | high usage | 幾乎 100% 使用 | high usage |
| B. 程式結束    | low usage  | --             | --         |

:::

#### 結論

從以上實驗可以看出，作業系統在背後做了非常多優化服務，包括 CPU 資源分配(即行程管理)、記憶體管理、虛擬記憶體管理、Disk I/O 管理等。
目前完成兩個作業後，我發現最大的困難總是在 I/O 處理速度，程式的平行度再高，也無法克服這個障礙，因此我認為，作業系統或許在 I/O 資源分配時，能夠提供一些 API 來設定優先度等等細節處理。

### 改善之處

由於此程式是假設 CSV 檔案大小可能超過記憶體大小下去設計的，因此限縮了記憶體使用資源，需要依靠大量的 IO 讀寫，而 IO 讀寫正是此程式的最大瓶頸。
在此提出一些可以改善之處：

1. 可以將程式設定成兩種模式，分別為大檔模式與小檔模式，於偵測檔案大小後做出判斷。
   - 大檔模式：限制 Memory 資源(過程參考上述程式架構)
   - 小檔模式：把全部資料讀進記憶體，再進行單一 thread 輸出
2. 對於"平行輸出至暫存檔"階段再進行優化，因為他是整個程式中執行最慢的部份(雖然我已經盡力ㄌ QQ)。

## Contact

### :email: e14066282@gs.ncku.edu.tw
