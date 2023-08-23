# -*- coding: utf-8 -*-

import os
import struct  

gcov_output_file = r"gcov_output.bin"

with open(gcov_output_file, "rb") as fd:
    content = fd.read()        
    while len(content) > 0:
        # 读取gcda文件名
        is_file_find = False
        for i in range(len(content)):
            if content[i] == 0:
                gcda_file_name = struct.unpack(str(i)+'s', content[:i])[0]
                content = content[i+1:]
                is_file_find = True
                break

        if not is_file_find:
            break

        if gcda_file_name == b"Gcov End":            
            break

        # 读取gcda数据长度
        gcda_data_size = struct.unpack('>I', content[:4])[0]
        content = content[4:]

        # 读取gcada数据内容
        gcda_data_buff = struct.unpack(str(gcda_data_size)+'s', content[:gcda_data_size])[0]
        content = content[gcda_data_size:]
        print( gcda_file_name)

        # 将数据写入到文件
        with open(gcda_file_name, "wb") as gcda:
            gcda.write(gcda_data_buff)

        
            
