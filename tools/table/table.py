# -*- coding:utf-8 -*-
'''
Created on 2014年10月19日

@author: jia
'''
import os
import logging
from collections import Counter
import xlrd

import log
import datatype

log.init_logger('table')
logger = logging.getLogger('table')


class Excel2Table(object):
    def __init__(self):
        self._template_start = "# " + "以下导表由程序生成，请勿修改".ljust(77, '-') + "\n"
        self._template_end = "\n" + "# " + "导表结束".ljust(77, '-') + "\n"

    def run(self, xls, out):
        data = xlrd.open_workbook(xls)
        sheet_num = len(data.sheets())

        context = ''
        for index in range(sheet_num):
            sheet = data.sheet_by_index(index)
            context += self.convert_sheet(sheet)
            context += "\n"
        context = context[:-1]

        self.write_table(context, out)

    def convert_sheet(self, sheet):
        sheetName = sheet.name
        nrows, ncols = sheet.nrows, sheet.ncols  # 本工作簿的行数和列数
        typeList = []
        nameList = []
        for col in range(ncols):
            typeList.append(sheet.cell_value(1, col))
            nameList.append(sheet.cell_value(2, col))

        if typeList[0].lower().endswith("key"):
            logger.error('excel表的第一列数据固定是key(键)类型的，并且是唯一值，不能手动设置成key类型')
            raise

        keyIndexList = [0]
        for idx, stype in enumerate(typeList):
            if stype.lower().endswith("key"):
                typeList[idx] = stype[:-3]
                keyIndexList.append(idx)

        def genline(indent, line, *arg):
            line = ' ' * 4 * indent + line % arg
            return line

        context = "%s = {\n" % sheetName
        isSkip = False
        isStart = False
        tarCol = 0
        indexNum = len(keyIndexList)
        for row in range(3, nrows):  # 逐行扫描
            index = 0  # 当前是第几级的key
            if isStart:
                for cnt, idx in enumerate(keyIndexList[::-1]):  # 进入下一行后，如果key类型的这一格不是是空的，就认为上个字典结束，封闭括号
                    value = sheet.cell_value(row, idx)
                    if value:
                        context += genline(indexNum - cnt, "},\n")
            else:
                isStart = True

            for col in range(ncols):  # 逐列扫描
                text = self.get_data(sheet, typeList[col], row, col)
                if isSkip and col < tarCol:
                    if col == tarCol - 1:
                        isSkip = False
                    continue
                if col == keyIndexList[index]:
                    value = sheet.cell_value(row, col)
                    if value:
                        context += genline(index + 1, "%s: {\n", text)
                    else:
                        if index < indexNum - 1:
                            index += 1
                            isSkip = True
                            tarCol = keyIndexList[index]
                            continue
                else:
                    context += genline(index + 2, "%s: %s,\n", repr(nameList[col]), text)

                if index < indexNum - 1:  # 进入下一级的字典嵌套
                    if col + 1 == keyIndexList[index + 1]:
                        index += 1

            if row == nrows - 1:
                for idx in range(indexNum):
                    context += genline(index - idx + 1, "},\n")
        context += "}\n"
        return context

    def get_data(self, sheet, stype, row, col):
        text = sheet.cell_value(row, col)
        value = datatype.convert_value(stype, text)
        return value

    def check_key(self, sheet, keyIndexList, nameList, nrows, ncols):  # 检查key是不是唯一的
        # 以key作为节点，构建一颗节点树，通过判断某节点是否有重复的子节点来判断是否有重复的key
        # TODO: 调整生成导表的算法，先扫描excel表，生成一个节点树，由这个节点树生成导表
        return True

    def write_table(self, context, out):
        if os.path.exists(out):
            with open(out, "r", encoding="utf8") as f:
                data = [line for line in f.readlines()]
        else:
            data = []
        text = ""
        text = "# -*- coding:utf-8 -*-\n"
        for line in data:
            if line == self._template_start:
                break
            if line == "# -*- coding:utf-8 -*-\n":
                continue
            text += line
        text = text + self._template_start + context + self._template_end
        with open(out, "w", encoding="utf8") as f:
            f.write(text)


def main():
    import configparser
    import argparse
    import shutil

    parser = argparse.ArgumentParser(description='sln2cmake.')
    parser.add_argument("-d", '--dir', type=str, default='', help="xls文件夹的绝对路径")
    parser.add_argument("-o", '--out', type=str, default='', help="输出文件夹的绝对路径")
    parser.add_argument("-c", '--config', type=str, default='', help="ini配置文件的绝对路径")
    parser.add_argument("-e", '--erase', action='store_true', default=False, help="生成导表前，是否删除输出文件夹里的所有文件")
    args = parser.parse_args()

    xls_dir = os.path.abspath(args.dir.strip())
    out_dir = os.path.abspath(args.out.strip())
    cfg_file = os.path.abspath(args.config.strip())

    if not os.path.exists(xls_dir):
        return
    if not os.path.exists(cfg_file):
        return
    if args.erase:
        if os.path.exists(out_dir):
            shutil.rmtree(out_dir)
        os.makedirs(out_dir)

    config = configparser.ConfigParser()
    try:
        config.read(cfg_file)
    except Exception as e:
        logger.exception('读取导表列表文件出错')
        return
    for xls, out in config['task'].items():
        xls = xls.encode('gbk').decode('utf8')
        out = out.encode('gbk').decode('utf8')
        logger.info('%s -> %s' % (xls, out))
        xls = os.path.abspath(os.path.join(xls_dir, xls))
        out = os.path.abspath(os.path.join(out_dir, out))

        obj = Excel2Table()
        obj.run(xls, out)

if __name__ == '__main__':
    main()
