#ifndef buffer_chain_template_hpp
#define buffer_chain_template_hpp
#include <stddef.h>

struct BufferChain_s;

typedef struct BufferChain_s BUfferChain, *BufferChainRef;

BufferChainRef BufferChain_new();
void BufferChain_free(BufferChainRef* this);
void BufferChain_append(BufferChainRef this, void* buf, int len);
void BufferChain_clear(BufferChainRef this);
int BufferChain_size(BufferChainRef this);

#ifdef nada
class BufferChain
{
    public:
        using SPtr = std::shared_ptr<BufferChain>;
        using CBuf = ContigBuffer;
        using AsioConstBufferSeq = std::vector<boost::asio::const_buffer>;
        using AsioMutableBufferSeq = std::vector<boost::asio::mutable_buffer>;

    private:
        using Factory = ContigBufferFactoryT;

        std::vector<CBuf::SPtr>     m_chain;
        AsioConstBufferSeq          m_asio_chain;
        std::size_t                 m_size;
        Factory&                    m_buf_factory;

    public:
        /** constructors and assignment operators */
        BufferChain(Factory& factory): m_buf_factory(factory)
        {
            m_chain = std::vector<ContigBuffer::SPtr>();
            m_size = 0;
        }
        BufferChain(BufferChain& other): m_buf_factory(other.m_buf_factory)
        {
            m_size = 0;
            for (ContigBuffer::SPtr mb_sptr: other.m_chain) {
                this->push_back(m_buf_factory.makeSPtr(*mb_sptr));
            }
        }
        BufferChain& operator =(BufferChain& other)
        {
            if(&other == this) {
                return *this;
            }
            m_size = 0;
            for (ContigBuffer::SPtr mb_sptr: other.m_chain) {
                this->push_back(m_buf_factory.makeSPtr(*mb_sptr));
            }
            return *this;
        }
        BufferChain(BufferChain&& other): m_buf_factory(other.m_buf_factory)
        {
            m_size = other.m_size;
            m_chain = std::move(other.m_chain);
            m_asio_chain = std::move(other.m_asio_chain);
            other.m_size = 0;
            // m_size = other.m_size;
            // m_chain = std::move(other.m_chain);
            // other.m_size = 0;
        }
        BufferChain& operator =(BufferChain&& other)
        {
            if (&other == this) {
                return *this;
            }
            std::swap(m_chain, other.m_chain);
            std::swap(m_size, other.m_size);
            return *this;
        }

        /** append data - either append to the final block or add a new block*/

        void append(void* buf, std::size_t len)
        {
            if (m_chain.size() > 0) {
                CBuf::SPtr last_mb = m_chain.at(m_chain.size()-1);
                if ((last_mb->capacity() - last_mb->size()) >= len) {
                    last_mb->append(buf, len);
        #define MARVIN_MK_CONSTBUF
        #ifdef MARVIN_MK_CONSTBUF
                    m_asio_chain.back() = boost::asio::const_buffer(last_mb->data(), last_mb->size());
        #endif
                    m_size += len;
                    return;
                }
            }
            std::size_t required_len = (len > 256*4*8) ? len+100 : 256*4*8;
            CBuf::SPtr new_mb = m_buf_factory.makeSPtr(required_len);
            new_mb->append(buf, len);
            this->push_back(new_mb);
        }
        void append(std::string str)
        {
            append((void*)str.c_str(), str.size());
        }
        void append(std::string& str)
        {
            append((void*)str.c_str(), str.size());
        }

        void push_back(CBuf::SPtr mb)
        {
            m_size += mb->size();
            m_chain.push_back(mb);
        // this is not working
        #ifdef MARVIN_MK_CONSTBUF
            m_asio_chain.emplace_back(boost::asio::const_buffer(mb->data(), mb->size()));
        #endif
        }
        void clear()
        {
            m_chain.clear();
            m_asio_chain.clear();
            m_size = 0;
        }
        std::size_t size()
        {
            return m_size;
        }
                
        std::size_t blocks()
        {
            return m_chain.size();
        }
        ContigBuffer& block_at(std::size_t index)
        {
            if (index >= m_chain.size()) {
                    MARVIN_THROW("index out of range");
            }
            return *(m_chain.at(index));
        }
        std::string to_string()
        {
            std::string s = "";
            for(CBuf::SPtr& mb : m_chain) {
                s += mb->toString();
            }
            return s;
        }
        CBuf::SPtr amalgamate()
        {
            CBuf::SPtr mb_final = m_buf_factory.makeSPtr(this->size());
            for(CBuf::SPtr& mb : m_chain) {
                mb_final->append(mb->data(), mb->size());
            }
            return mb_final;
        }


        void createAsioBufferSequence()
        {
        #ifdef MARVIN_MK_CONSTBUF
            return;
        #else
            m_asio_chain = AsioConstBufferSeq();
            for(auto mb: m_chain) {
                m_asio_chain.emplace_back(boost::asio::const_buffer(mb->data(), mb->size()));
            }
        #endif
        }

        friend std::ostream& operator <<(std::ostream &os, BufferChain& b)
        {
            if(b.size() == 0){
                os << "Empty ";
            }else{
                std::string s = b.to_string();
                os << std::endl << "BufferChain{ length: " << b.size() << "content: [" << s << "]}";
            }
            return os;
        }
        /**
        * converts a Bufferchain to a boost buffer for use in async write calls
        * thus need const_bufer_seq
        */
        AsioConstBufferSeq& asio_buffer_sequence()
        {
            createAsioBufferSequence();
            return this->m_asio_chain;
        }
        friend AsioConstBufferSeq& buffer_chain_to_const_buffer_sequence(BufferChain& bchain)
        {
            bchain.createAsioBufferSequence();
            return bchain.m_asio_chain;
        }
        friend AsioConstBufferSeq& buffer_chain_to_mutable_buffer_sequence(BufferChain& bchain)
        {
            bchain.createAsioBufferSequence();
            return bchain.m_asio_chain;
        }

};

inline std::string buffersequence_to_string(BufferChain::AsioConstBufferSeq& mutbufs)
{
    std::string s;
    for(auto& m: mutbufs) {
        s = s + std::string((char*)m.data(), m.size());
    }
    return s;
}
#endif
#endif
