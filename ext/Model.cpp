
#include "Model.h"

#include <skse64/GameStreams.h>

namespace Util
{
    namespace Model
    {
        static bool MakePath(
            const char* a_prefix,
            const char* a_path,
            char (&a_buffer)[MAX_PATH],
            const char *&a_out)
        {
            if (!a_path) {
                return false;
            }

            auto prefixLen = std::strlen(a_prefix);
            auto pathLen = std::strlen(a_path);

            if (_strnicmp(a_prefix, a_path, std::min(prefixLen, pathLen)) == 0) {
                a_out = a_path;
            }
            else {
                _snprintf_s(a_buffer, _TRUNCATE, "%s\\%s", a_prefix, a_path);
                a_out = a_buffer;
            }

            return true;
        }

        bool ModelLoader::Load(
            const char* a_path)
        {
            using namespace Stream;

            char path_buffer[MAX_PATH];
            const char* path;

            if (!MakePath("meshes", a_path, path_buffer, path)) {
                return false;
            }

            BSResourceNiBinaryStream binaryStream(path);
            if (!binaryStream.IsValid()) {
                return false;
            }

            Stream::NiStreamWrapper stream;

            stream->LoadStream(&binaryStream);

            if (!stream->m_rootObjects.m_data) {
                return false;
            }

            m_stream = std::move(stream);

            return true;
        }
    }
}