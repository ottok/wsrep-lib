//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

#ifndef WSREP_MOCK_SERVER_CONTEXT_HPP
#define WSREP_MOCK_SERVER_CONTEXT_HPP

#include "wsrep/server_context.hpp"
#include "mock_client_context.hpp"
#include "mock_provider.hpp"

#include "wsrep/compiler.hpp"

namespace wsrep
{
    class mock_server_context : public wsrep::server_context
    {
    public:
        mock_server_context(const std::string& name,
                            const std::string& id,
                            enum wsrep::server_context::rollback_mode rollback_mode)
            : wsrep::server_context(mutex_, cond_,
                                    name, id, "", "./", rollback_mode)
            , mutex_()
            , cond_()
            , provider_(*this)
            , client_service_(provider_)
            , last_client_id_(0)
        { }
        wsrep::mock_provider& provider() const
        { return provider_; }
        wsrep::client_context* local_client_context()
        {
            return new wsrep::mock_client_context(
                *this, client_service_, ++last_client_id_,
                wsrep::client_context::m_local);
        }
        wsrep::client_context* streaming_applier_client_context()
        {
            return new wsrep::mock_client_context(
                *this, client_service_, ++last_client_id_,
                wsrep::client_context::m_high_priority);
        }
        void release_client_context(wsrep::client_context* client_context)
        {
            delete client_context;
        }

        void log_dummy_write_set(wsrep::client_context&,
                                 const wsrep::ws_meta&)
            WSREP_OVERRIDE
        {
        }
        void on_connect() WSREP_OVERRIDE { }
        void wait_until_connected() WSREP_OVERRIDE { }
        void on_view(const wsrep::view&) WSREP_OVERRIDE { }
        void on_sync() WSREP_OVERRIDE { }
        bool sst_before_init() const WSREP_OVERRIDE { return false; }
        std::string on_sst_required() WSREP_OVERRIDE { return ""; }
        void on_sst_request(const std::string&,
                            const wsrep::gtid&,
                            bool) WSREP_OVERRIDE { }
        void background_rollback(wsrep::client_context& client_context)
            WSREP_OVERRIDE
        {
            client_context.before_rollback();
            client_context.after_rollback();
        }
        // void sst_received(const wsrep_gtid_t&, int) WSREP_OVERRIDE { }
        // void on_apply(wsrep::transaction_context&) { }
        // void on_commit(wsrep::transaction_context&) { }
        wsrep::mock_client_service& client_service()
        {
            return client_service_;
        }
    private:
        wsrep::default_mutex mutex_;
        wsrep::default_condition_variable cond_;
        mutable wsrep::mock_provider provider_;
        wsrep::mock_client_service client_service_;
        unsigned long long last_client_id_;
    };
}

#endif // WSREP_MOCK_SERVER_CONTEXT_HPP
