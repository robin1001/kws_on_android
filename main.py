def binary_search(arr, x):
    low = 0
    high = len(arr) - 1
    while low <= high:
        mid = (low + high) // 2
        if arr[mid] == x:
            return mid
        elif arr[mid] > x:
            high = mid - 1
        else:
            low = mid + 1
    return -1
 
# 示例使用
arr = [1, 2, 3, 4, 5, 6, 7, 8, 9]
x = 6
index = binary_search(arr, x)
if index != -1:
    print(f"元素 {x} 的索引为 {index}")
else:
    print(f"数组中不存在 {x}")
