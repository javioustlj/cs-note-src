
def write_file(file_name, content):
    # 以文本模式打开文件，如果文件不存在则创建，若存在则覆盖内容
    with open(file_name, 'w', encoding='utf-8') as file:
        file.write(content)


def analyze(text):
    lines = text.split('\n')

    new_content = ''
    title = ''
    yulu = ''
    comment = ''
    cnt = 0
    for line in lines:
        if len(line):
            if cnt == 0:
                title = line.strip()
            elif cnt == 1:
                yulu = line.strip()
            else:
                if comment:
                    comment +=  ' \\\\\n'
                comment += line.strip().replace("〈", "").replace("〉", "")
            cnt += 1
        else:
            # print(f"{title}\n{yulu}\n{comment}\n")
            print(f"\\begin{{yulu}}{{{title}}}\n{yulu}\n\\begin{{comments}}\n{comment}\n\\end{{comments}}\n\\end{{yulu}}\n")
            new_content += f"\\begin{{yulu}}{{{title}}}\n{yulu}\n\\begin{{comments}}\n{comment}\n\\end{{comments}}\n\\end{{yulu}}\n" + '\n'
            cnt = 0
            title = ''
            yulu = ''
            comment = ''
    return new_content





with open('1.txt', 'r', encoding='utf-8') as f:
    content = f.read()
    new_content = analyze(content)
    write_file('2.tex', new_content)


#mw-content-text > div.mw-content-ltr.mw-parser-output > p:nth-child(26) > small:nth-child(2)
