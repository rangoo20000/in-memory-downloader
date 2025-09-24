#include <Windows.h>
#include <Urlmon.h>   // URLOpenBlockingStreamW()
#include <atlbase.h>  // CComPtr
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Ole32.lib")

// ================= GLOBAL VARIABLES =================
char* g_downloadedData = nullptr;   
size_t g_downloadedSize = 0;        


struct ComInit {
    HRESULT hr;
    ComInit() : hr(::CoInitialize(nullptr)) {}
    ~ComInit() { if (SUCCEEDED(hr)) ::CoUninitialize(); }
};


size_t in_memory_downloader(const std::wstring& url) {
    // flushing variable
    if (g_downloadedData) {
        delete[] g_downloadedData;
        g_downloadedData = nullptr;
        g_downloadedSize = 0;
    }

    CComPtr<IStream> pStream;
    HRESULT hr = URLOpenBlockingStreamW(nullptr, url.c_str(), &pStream, 0, nullptr);
    if (FAILED(hr)) {
        return 0;
    }

    std::vector<char> buffer;
    char temp[4096];
    ULONG bytesRead = 0;

    while (SUCCEEDED(pStream->Read(temp, sizeof(temp), &bytesRead)) && bytesRead > 0) {
        buffer.insert(buffer.end(), temp, temp + bytesRead);
    }

    if (buffer.empty()) return 0;

    g_downloadedSize = buffer.size();
    g_downloadedData = new char[g_downloadedSize];
    memcpy(g_downloadedData, buffer.data(), g_downloadedSize);

    return g_downloadedSize;
}

int main() {
    ComInit init;

    std::cout << "Enter URL:\n> ";
    std::string urlStr;
    std::getline(std::cin, urlStr);

    if (urlStr.empty()) {
        std::cout << "No URL entered. Exiting...\n";
        return 0;
    }

    std::wstring url(urlStr.begin(), urlStr.end());

    size_t size = in_memory_downloader(url);

    if (size > 0 && g_downloadedData) {
        std::cout << "✅ Download successful.\n";
        std::cout << "Bytes received: " << size << "\n";
        std::cout << "Global pointer: g_downloadedData\n";
        std::cout << "Memory address: " << static_cast<void*>(g_downloadedData) << "\n";
    }
    else {
        std::cout << "❌ Download failed.\n";
    }

    
    if (g_downloadedData) {
        delete[] g_downloadedData;
        g_downloadedData = nullptr;
        g_downloadedSize = 0;
    }

    return 0;
}
