#ifndef YSL_EXTENSION_CURL_HH
#define YSL_EXTENSION_CURL_HH

#include <curl/curl.h>

#include "../environment.hh"
#include "../util.hh"

#define PARAM_AE const std::vector <std::string>& args, Environment& env
#define PARAM_A  const std::vector <std::string>& args, Environment&
#define PARAM_E  const std::vector <std::string>&, Environment& env
#define PARAM_0  const std::vector <std::string>&, Environment&


namespace YSL {
	namespace Extensions {
		namespace Curl {
			static CURL* handle;

			size_t CurlWriteFunction(
				char* buf, size_t size, size_t nmemb, void* userp
			) {
			    auto vec = static_cast <std::vector<int32_t>*>(userp);
			    
			    for (size_t i = 0; i < size * nmemb; ++i) {
			    	vec->push_back(buf[i]);
			    }
			    
			    return nmemb * size;
			}
			
			std::vector <int> Init(PARAM_AE) {
				env.Assert(args.empty(), "Curl.Init: Requires 0 arguments");
			
				if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
					env.Assert(false, "curl_global_init failed");
				}
			
				handle = curl_easy_init();
				if (!handle) {
					env.Assert(false, "curl_easy_init returned NULL");
				}

				return {};
			}

			std::vector <int> Free(PARAM_AE) {
				env.Assert(args.empty(), "Curl.Free: Requires 0 arguments");
				
				curl_easy_cleanup(handle);
				curl_global_cleanup();

				return {};
			}

			std::vector <int> Download(PARAM_AE) {
				env.Assert(args.size() == 1, "Curl.Download: Requires 1 argument");
			
				std::vector <int32_t> downloadedData;

				curl_easy_reset(handle);
				curl_easy_setopt(
					handle, CURLOPT_URL, args[0].c_str()
				);
				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlWriteFunction);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA,     &downloadedData);
				curl_easy_setopt(handle, CURLOPT_FAILONERROR,   true);

				auto rc = curl_easy_perform(handle);
				if (rc != CURLE_OK) {
					env.Assert(
						false,
						std::string("curl_easy_perform failed: ") +
						curl_easy_strerror(rc)
					);
				}

				return downloadedData;
			}
			
			Extension BuildExtension() {
				Extension ext;

				ext.name                  = "curl";
				ext.functions["init"]     = Init;
				ext.functions["free"]     = Free;
				ext.functions["download"] = Download;

				return ext;
			}
		}
	}
}

#endif
