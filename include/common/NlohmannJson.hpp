//
// Created by robin on 08.01.2021.
//

#ifdef INCLUDE_NLOHMANN_JSON_HPP_

#ifndef TILESON_NLOHMANNJSON_HPP
#define TILESON_NLOHMANNJSON_HPP

namespace tson
{
    class NlohmannJson : public tson::IJson
    {
        public:
            inline NlohmannJson() = default;

            IJson &operator[](std::string_view key) override
            {
                if(m_arrayCache.count(key.data()) == 0)
                    m_arrayCache[key.data()] = std::make_unique<NlohmannJson>(&m_json->operator[](key.data()).front());

                return *m_arrayCache[key.data()].get();
            }

            inline explicit NlohmannJson(nlohmann::json *json) : m_json {json}
            {

            }

            //inline explicit NlohmannJson(const fs::path &path)
            //{
            //    parse(path);
            //}
//
            //inline explicit NlohmannJson(const void *data, size_t size)
            //{
            //    parse(data, size);
            //}

            inline IJson& at(std::string_view key) override
            {
                if(m_arrayCache.count(key.data()) == 0)
                    m_arrayCache[key.data()] = std::make_unique<NlohmannJson>(&m_json->operator[](key.data()).front());

                return *m_arrayCache[key.data()].get();
            }

            inline IJson& at(size_t pos) override
            {
                if(m_arrayPosCache.count(pos) == 0)
                    m_arrayPosCache[pos] = std::make_unique<NlohmannJson>(&m_json->at(pos));

                return *m_arrayPosCache[pos].get();
                //return std::make_unique<NlohmannJson>(&m_json->at(pos));
            }

            inline std::vector<std::unique_ptr<IJson>> & array(std::string_view key) override
            {
                if(m_arrayListDataCache.count(key.data()) == 0)
                {
                    if (m_json->count(key.data()) > 0 && m_json->operator[](key.data()).is_array())
                    {
                        std::for_each(m_json->operator[](key.data()).begin(), m_json->operator[](key.data()).end(), [&](nlohmann::json &item)
                        {
                            nlohmann::json *ptr = &item;
                            m_arrayListDataCache[key.data()].emplace_back(std::make_unique<NlohmannJson>(ptr));
                            //m_arrayListRefCache[key.data()].emplace_back(*m_arrayListDataCache[key.data()].at(m_arrayListDataCache[key.data()].size() - 1));
                        });
                    }
                }


                return m_arrayListDataCache[key.data()];
            }

            inline size_t size() const override
            {
                return m_json->size();
            }

            inline bool parse(const fs::path &path) override
            {
                if (fs::exists(path) && fs::is_regular_file(path))
                {
                    m_data = std::make_unique<nlohmann::json>();
                    std::ifstream i(path.u8string());
                    try
                    {
                        i >> *m_data;
                        m_json = m_data.get();
                    }
                    catch (const nlohmann::json::parse_error &error)
                    {
                        std::string message = "Parse error: ";
                        message += std::string(error.what());
                        message += std::string("\n");
                        std::cerr << message;
                        return false;
                    }
                    return true;
                }
                return false;
            }

            inline bool parse(const void *data, size_t size) override
            {
                m_data = std::make_unique<nlohmann::json>();
                tson::MemoryStream mem{(uint8_t *) data, size};
                try
                {
                    mem >> *m_data;
                    m_json = m_data.get();
                }
                catch (const nlohmann::json::parse_error &error)
                {
                    std::string message = "Parse error: ";
                    message += std::string(error.what());
                    message += std::string("\n");
                    std::cerr << message;
                    return false;
                }
                return true;
            }

            [[nodiscard]] inline size_t count(std::string_view key) const override
            {
                return m_json->operator[](key.data()).count(key);
            }

            [[nodiscard]] inline bool any(std::string_view key) const override
            {
                return m_json->operator[](key.data()).count(key) > 1;
            }

            [[nodiscard]] inline bool isArray() const override
            {
                return m_json->is_array();
            }

            [[nodiscard]] inline bool isObject() const override
            {
                return m_json->is_object();
            }

            [[nodiscard]] inline bool isNull() const override
            {
                return m_json->is_null();
            }

        protected:
            [[nodiscard]] inline int32_t getInt32(std::string_view key) const override
            {
                return m_json->operator[](key.data()).get<int32_t>();
            }

            [[nodiscard]] inline uint32_t getUInt32(std::string_view key) const override
            {
                return m_json->operator[](key.data()).get<uint32_t>();
            }

            [[nodiscard]] inline int64_t getInt64(std::string_view key) const override
            {
                return m_json->operator[](key.data()).get<int64_t>();
            }

            [[nodiscard]] inline uint64_t getUInt64(std::string_view key) const override
            {
                return m_json->operator[](key.data()).get<uint64_t>();
            }

            [[nodiscard]] inline double getDouble(std::string_view key) const override
            {
                return m_json->operator[](key.data()).get<double>();
            }

            [[nodiscard]] inline std::string getString(std::string_view key) const override
            {
                return m_json->operator[](key.data()).get<std::string>();
            }

            [[nodiscard]] inline bool getBool(std::string_view key) const override
            {
                return m_json->operator[](key.data()).get<bool>();
            }

            [[nodiscard]] inline int32_t getInt32() const override
            {
                return m_json->get<int32_t>();
            }

            [[nodiscard]] inline uint32_t getUInt32() const override
            {
                return m_json->get<uint32_t>();
            }

            [[nodiscard]] inline int64_t getInt64() const override
            {
                return m_json->get<int64_t>();
            }

            [[nodiscard]] inline uint64_t getUInt64() const override
            {
                return m_json->get<uint64_t>();
            }

            [[nodiscard]] inline double getDouble() const override
            {
                return m_json->get<double>();
            }

            [[nodiscard]] inline std::string getString() const override
            {
                return m_json->get<std::string>();
            }

            [[nodiscard]] inline bool getBool() const override
            {
                return m_json->get<bool>();
            }

        private:
            nlohmann::json *m_json = nullptr;
            std::unique_ptr<nlohmann::json> m_data = nullptr; //Only used if this is the owner json!

            //Cache!
            std::map<std::string, std::unique_ptr<NlohmannJson>> m_arrayCache;
            std::map<size_t, std::unique_ptr<NlohmannJson>> m_arrayPosCache;
            std::map<std::string, std::vector<std::unique_ptr<IJson>>> m_arrayListDataCache;
            //std::map<std::string, std::vector<std::reference_wrapper<IJson>>> m_arrayListRefCache;
            //std::vector<IJson &>

    };
}
#endif //TILESON_NLOHMANNJSON_HPP

#endif //INCLUDE_NLOHMANN_JSON_HPP_