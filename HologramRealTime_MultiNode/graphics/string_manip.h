#ifndef __STRING_MANIP_H
#define __STRING_MANIP_H

#include <string>
#include <vector>
namespace graphics
{
	std::basic_string<wchar_t> ToLower(const std::basic_string<wchar_t>& source);
	std::basic_string<wchar_t> ToUpper(const std::basic_string<wchar_t>& source);
	std::basic_string<wchar_t> ReplaceAll(const std::basic_string<wchar_t> &str, const std::basic_string<wchar_t>& pattern, const std::basic_string<wchar_t>& replace);
	// c:\a\b.txt -> c:/a/
	// c:\a\b\c\d -> c:/a/b/c �� ��ȯ�Ѵ�.
	std::basic_string<wchar_t> GetDirectoryPath(const std::basic_string<wchar_t>& path);
	// ����� ������ �ܰ迡 �ִ� ���丮 ���̳� ���ϸ��� ��ȯ�Ѵ�. ���ϸ��� Ȯ���� ���Ը� �����Ѵ�.
	std::basic_string<wchar_t> GetPathLeafName(const std::basic_string<wchar_t>& path);

	void SeparateNameAndExt(const std::basic_string<wchar_t>& path, std::basic_string<wchar_t>& file_name, std::basic_string<wchar_t>& ext);

	// ��δ� �ݵ�� ���丮���� �Ѵ�.
	std::basic_string<wchar_t> AbsolutePathToRelativePath(const std::basic_string<wchar_t>& absolutePath, const std::basic_string<wchar_t>& targetPath);

	void get_tokens(std::string input, std::vector<std::string>& tokens, const char* delim);

	void separate_string(std::string input, std::string& out1, std::string& out2, const char* delim);

	void stripoff_blank(std::string& input, std::string& out);

	std::string to_utf8(const std::wstring& input);
	std::wstring to_utf16(const std::string& input);
}

#endif