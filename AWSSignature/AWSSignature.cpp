// AWSSignature.cpp : 
//

#include <iostream>

#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>

#include <aws/external/gtest.h>
#include <aws/core/auth/AWSAuthSigner.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/http/standard/StandardHttpRequest.h>
#include <aws/core/http/standard/StandardHttpResponse.h>
#include <aws/core/platform/FileSystem.h>
#include <aws/core/platform/Platform.h>
#include <aws/core/utils/StringUtils.h>
#include <fstream>

using namespace Aws::Client;
using namespace Aws::Utils;
using namespace Aws::Http;

static const char ALLOC_TAG[] = "AwsAuthV4SignerTest";
static const char EMPTY_STRING_SHA256[] = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
static const char UNSIGNED_PAYLOAD[] = "UNSIGNED-PAYLOAD";

static bool SignPayload(AWSAuthV4Signer::PayloadSigningPolicy policy, Aws::Http::Scheme scheme, bool requestSignPayload/*sign flag in request*/)
{
    if (scheme == Aws::Http::Scheme::HTTPS)
    {
        if (policy == AWSAuthV4Signer::PayloadSigningPolicy::Never || (policy == AWSAuthV4Signer::PayloadSigningPolicy::RequestDependent && requestSignPayload == false))
        {
            return false;
        }
    }
    return true;
}

int main()
{
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Http::Scheme scheme = Aws::Http::Scheme::HTTPS;
        bool requestSignPayload = false;
        bool signPayload = SignPayload(AWSAuthV4Signer::PayloadSigningPolicy::Never, scheme, requestSignPayload);
        auto request = Standard::StandardHttpRequest(scheme == Aws::Http::Scheme::HTTP ? "http://test.com/query?key=val" : "https://test.com/query?key=val", Aws::Http::HttpMethod::HTTP_GET);

        std::shared_ptr<Aws::Auth::AWSCredentialsProvider> credProvider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(ALLOC_TAG, "AKIDEXAMPLE", "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY");
        AWSAuthV4Signer signer(credProvider, "service", "us-east-1", AWSAuthV4Signer::PayloadSigningPolicy::Never, false);
        Aws::String a0 = credProvider->GetAWSCredentials().GetSessionToken();
        
        signer.SignRequest(request, requestSignPayload);
        Aws::String a1 = request.GetAuthorization();
        std::cout << a1 << std::endl;
        Aws::String a2 = request.GetAwsAuthorization();
        std::cout << a2 << std::endl;
    }
    Aws::ShutdownAPI(options);
}

