import requests
from bs4 import BeautifulSoup, Tag


def fetch(url):
    try:
        response = requests.get(url)
        if response.status_code == 200:
            print('请求成功')
        else:
            print(f'请求失败，状态码：{response.status_code}')
        return response
    except requests.RequestException as e:
        print(f'请求过程中发生错误：{e}')


def write_file(file_name, content):
    # 以文本模式打开文件，如果文件不存在则创建，若存在则覆盖内容
    with open(file_name, 'w', encoding='utf-8') as file:
        file.write(content)


def analyze(html):
    soup = BeautifulSoup(html, 'html.parser')
    content_div = soup.find(attrs={"property": "mw:PageProp/toc"})
    children_count = len(list(content_div.children))
    print(f"content_div 的直接子节点个数为: {children_count}")
    print(f"content_div 的类型: {type(content_div)}")
    print(f"Tag 的类型: {Tag}")
    # print(content_div)
    cnt = 0
    for child in content_div.children:
        # print(child)
        # print(f"child 的类型: {type(child)}")
        # cnt += 1
        # if cnt == 3:
        #     print(f"cnt: {cnt}, child: ", child)
        #     break
        # break
        if isinstance(child, Tag):
            class_value = child.get('class', [])
            # if class_value:
            #     print(child.get('class', []))
            #     print(child.get('class'))
            #     print(' '.join(child.get('class', [])))
            # else:
            #     print("子节点没有 class 属性。")
            # print("isinstance")
            # if 'mw-heading mw-heading1' in ''.join(child.get('class', [])):
            if 'mw-heading' in class_value and 'mw-heading1' in class_value:
                # 若子节点的 class 为 "mw-heading mw-heading1"，做相应处理
                # print("子节点 class 为 'mw-heading mw-heading1'，处理该节点：", child.text)
                h = child.find('h1')
                print(f"\chapter{{{h.text}}}")
            elif 'mw-heading' in class_value and 'mw-heading2' in class_value:
                # print(child.text)
                # 检查子节点是否为 Tag 类型，避免处理文本节点等非标签节点
                h = child.find('h2')
                title = h
                print(title)
            if hasattr(child, 'name') and child.name == 'p':
                # print(child.text)
                br_tag =
        # break



# 要爬取的网页URL
# target_url = 'https://zh.wikisource.org/zh-hans/%E5%B9%BD%E5%A4%A2%E5%BD%B1'
# response = fetch(target_url)
# write_file('webpage.html', response.text)

with open('webpage.html', 'r', encoding='utf-8') as f:
    content = f.read()
    analyze(content)



#mw-content-text > div.mw-content-ltr.mw-parser-output > p:nth-child(26) > small:nth-child(2)
