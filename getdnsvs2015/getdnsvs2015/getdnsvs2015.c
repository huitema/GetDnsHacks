/*
 * getdnsvs2015.c : Basic test of a VS Studio port of GetDNS.
 */

#include "stdafx.h"
#include <Config.h>
 
 /*
  * test code copied from the GetDNS documentation.
  */
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <getdns/getdns.h>
#include <getdns/getdns_extra.h>

#ifndef GETDNS_STR_IPV4
#define GETDNS_STR_IPV4 "IPv4"
#endif

#define GETDNS_STR_IPV6 "IPv6"

#define GETDNS_STR_ADDRESS_TYPE "address_type"
#define GETDNS_STR_ADDRESS_DATA "address_data"
#define GETDNS_STR_PORT "port"
#define GETDNS_STR_TLS_PORT "tls_port"
#define GETDNS_STR_AUTH_NAME "tls_auth_name"

uint64_t _getdns_get_time_as_uintt64();

/* Set up the callback function, which will also do the processing of the results */
void callback(getdns_context        *context,
	getdns_callback_type_t callback_type,
	getdns_dict           *response,
	void                  *userarg,
	getdns_transaction_t   transaction_id)
{
	getdns_return_t r;  /* Holder for all function returns */
	uint32_t        status;
	getdns_bindata  *address_data;
	char            *first = NULL, *second = NULL;

	(void)context; /* unused parameter */

	printf("Callback for query \"%s\" with request ID %"PRIu64".\n"
		, (char *)userarg, transaction_id);

	switch (callback_type) {
	case GETDNS_CALLBACK_CANCEL:
		printf("Transaction with ID %"PRIu64" was cancelled.\n", transaction_id);
		return;
	case GETDNS_CALLBACK_TIMEOUT:
		printf("Transaction with ID %"PRIu64" timed out.\n", transaction_id);
		return;
	case GETDNS_CALLBACK_ERROR:
		printf("An error occurred for transaction ID %"PRIu64".\n", transaction_id);
		return;
	default: break;
	}
	assert(callback_type == GETDNS_CALLBACK_COMPLETE);

	if ((r = getdns_dict_get_int(response, "status", &status)))
		fprintf(stderr, "Could not get \"status\" from reponse");

	else if (status != GETDNS_RESPSTATUS_GOOD)
		fprintf(stderr, "The search had no results, and a return value of %"PRIu32".\n", status);

	else if ((r = getdns_dict_get_bindata(response, "/just_address_answers/0/address_data", &address_data)))
		fprintf(stderr, "Could not get first address");

	else if (!(first = getdns_display_ip_address(address_data)))
		fprintf(stderr, "Could not convert first address to string\n");

	else if ((r = getdns_dict_get_bindata(response, "/just_address_answers/1/address_data", &address_data)))
		fprintf(stderr, "Could not get second address");

	else if (!(second = getdns_display_ip_address(address_data)))
		fprintf(stderr, "Could not convert second address to string\n");

	if (first) {
		printf("The address is %s\n", first);
		free(first);
	}
	if (second) {
		printf("The address is %s\n", second);
		free(second);
	}
	if (r) {
		assert(r != GETDNS_RETURN_GOOD);
		fprintf(stderr, ": %d\n", r);
	}
	getdns_dict_destroy(response);
}

#ifdef PSEUDO_SYNC
struct pseudo_sync_arg
{
    getdns_callback_type_t callback_type;
    getdns_return_t r;

    int size;
    unsigned char addr[16];
};

/* We got a response. Now, copy it into the userarg, so it can be
used in pseudo sync mode */
void simple_callback(getdns_context        *context,
    getdns_callback_type_t callback_type,
    getdns_dict           *response,
    void                  *userarg,
    getdns_transaction_t   transaction_id)
{
    struct pseudo_sync_arg * pa = (struct pseudo_sync_arg*) userarg;
    getdns_bindata  *address_data;
    char            *first = NULL, *second = NULL;

    (void)context; /* unused parameter */

    pa->callback_type = callback_type;
    pa->size = 0;

    if (callback_type != GETDNS_CALLBACK_COMPLETE)
    {
        pa->r = GETDNS_RETURN_GENERIC_ERROR;
    }
    else
    {
        pa->r = getdns_dict_get_bindata(response, 
            "/just_address_answers/0/address_data", &address_data);

        if (pa->r == 0)
        {
            if (address_data->size <= 16)
            {
                pa->size = address_data->size;
                memcpy(pa->addr, address_data->data, address_data->size);
            }
        }
    }
}

getdns_return_t pseudo_sync_call(getdns_context *context,
    char * query_name, struct pseudo_sync_arg * pa)
{
    getdns_return_t r = 0;
    static getdns_transaction_t transaction_id = 0xBABA;
    getdns_dict         *extensions = NULL;

    transaction_id++;
    pa->callback_type = GETDNS_CALLBACK_ERROR;
    pa->r = GETDNS_RETURN_GENERIC_ERROR;
    pa->size = 0;

    if ((r = getdns_address(context, query_name, extensions
        , (void*) pa, &transaction_id, simple_callback)))
        fprintf(stderr, "Error scheduling asynchronous request");

    else {
        getdns_context_run(context);
        r = pa->r;
    }

    return r;
}
#endif

getdns_return_t
show_response(
    getdns_dict           *response)
{
    getdns_return_t r;  /* Holder for all function returns */
    uint32_t        status;
    getdns_bindata  *address_data;
    char            *first = NULL, *second = NULL;

    if ((r = getdns_dict_get_int(response, "status", &status)))
        fprintf(stderr, "Could not get \"status\" from reponse");

    else if (status != GETDNS_RESPSTATUS_GOOD)
        fprintf(stderr, "The search had no results, and a return value of %"PRIu32".\n", status);

    else if ((r = getdns_dict_get_bindata(response, "/just_address_answers/0/address_data", &address_data)))
        fprintf(stderr, "Could not get first address");

    else if (!(first = getdns_display_ip_address(address_data)))
        fprintf(stderr, "Could not convert first address to string\n");

    else if ((r = getdns_dict_get_bindata(response, "/just_address_answers/1/address_data", &address_data)))
        fprintf(stderr, "Could not get second address");

    else if (!(second = getdns_display_ip_address(address_data)))
        fprintf(stderr, "Could not convert second address to string\n");

    if (first) {
        printf("The address is %s\n", first);
        free(first);
    }
    if (second) {
        printf("The address is %s\n", second);
        free(second);
    }
    if (r) {
        assert(r != GETDNS_RETURN_GOOD);
        fprintf(stderr, ": %d\n", r);
    }
    getdns_dict_destroy(response);

    return r;
}

getdns_return_t
InitializeGetDNSContext(getdns_context** context)
{
    /* Create the DNS context for this call */
    struct getdns_context *this_context = NULL;
    getdns_return_t r =
        getdns_context_create(&this_context, 1);

    *context = NULL;

    if (r != GETDNS_RETURN_GOOD) {
        fprintf(stderr, "Trying to create the context failed: %d", r);
    }

    if (r == GETDNS_RETURN_GOOD)
    {
        r = getdns_context_set_resolution_type(this_context, GETDNS_RESOLUTION_STUB);
        if (r != GETDNS_RETURN_GOOD) {
            fprintf(stderr, "Trying to set stub mode failed: %d", r);
        }
    }


    if (r == GETDNS_RETURN_GOOD)
    {
        r = getdns_context_set_dns_transport(this_context, GETDNS_TRANSPORT_TLS_ONLY_KEEP_CONNECTIONS_OPEN);

        if (r != GETDNS_RETURN_GOOD) {
            fprintf(stderr, "Trying to set tls connection keep failed: %d", r);
        }
    }

    if (r == GETDNS_RETURN_GOOD)
    {
        r = getdns_context_set_idle_timeout(this_context, 10000);
        if (r != GETDNS_RETURN_GOOD) {
            fprintf(stderr, "Trying to set keep alive failed: %d", r);
        }
    }
    

    if (r == GETDNS_RETURN_GOOD) {
        *context = this_context;
    }
    else if (this_context != NULL)
    {
        getdns_context_destroy(this_context);
    }

    return(r);
}

getdns_return_t
InitializeServerAddress(getdns_context* context,
    getdns_dict * response,
    uint32_t port,
    char* tls_auth_name)
{
    getdns_list *upstream_list = NULL;
    getdns_return_t r = 0;
    getdns_dict * server_dict = NULL;
    struct getdns_bindata bindata;
    struct getdns_bindata *addressdata = NULL;
    struct getdns_bindata addresstype;

    server_dict = getdns_dict_create_with_context(context);
    if (server_dict == NULL)
    {
        fprintf(stderr, "Allocate IP dictionary failed");
        r = GETDNS_RETURN_MEMORY_ERROR;
    }


    if (r == 0)
    {
        /*
        char *dict_str = getdns_pretty_print_dict(response);
        if (dict_str != NULL)
        {
            printf("Server's address is:\n%s\n", dict_str);
            free(dict_str);
        }
        */

        if (r = getdns_dict_get_bindata(response, "/just_address_answers/0/address_data", &addressdata))
            fprintf(stderr, "Could not get first address\n");

        if (r == 0)
        {
            if (addressdata->size == 16)
            {
                /* Set the IPv6 address */
                addresstype.size = strlen(GETDNS_STR_IPV6) + 1;
                addresstype.data = (uint8_t*)GETDNS_STR_IPV6;
            }
            else if (addressdata->size == 4)
            {
                /* Set the IPv4 address */
                addresstype.size = strlen(GETDNS_STR_IPV4) + 1;
                addresstype.data = (uint8_t*)GETDNS_STR_IPV4;
            }
            else
            {
                r = GETDNS_RETURN_GENERIC_ERROR;
                fprintf(stderr, "Incorrect first address");
            }

            if (r == 0)
            {
                r = getdns_dict_set_bindata(server_dict, GETDNS_STR_ADDRESS_TYPE, &addresstype);
                if (r != 0)
                    fprintf(stderr, "Error: set ip address type");
            }

            if (r == 0)
            {
                r = getdns_dict_set_bindata(server_dict, GETDNS_STR_ADDRESS_DATA, addressdata);

                if (r != 0)
                    fprintf(stderr, "Error: set address data");
            }
        }
    }

    if (r == 0)
    {
        r = getdns_dict_set_int(server_dict, GETDNS_STR_TLS_PORT, port); /* TODO: TLS PORT*/

        if (r != 0)
            fprintf(stderr, "Error: set addr port");
    }

    if (r == 0 && tls_auth_name != NULL)
    {
        bindata.size = strlen(tls_auth_name) + 1;
        bindata.data = tls_auth_name;
        r = getdns_dict_set_bindata(server_dict, GETDNS_STR_AUTH_NAME, &bindata);

        if (r != 0)
            fprintf(stderr, "Error: set tls auth name");
    }

    if (r == 0)
    {
        upstream_list = getdns_list_create_with_context(context);
        if (upstream_list == NULL)
        {
            fprintf(stderr, "Error: Allocate lists");
        }
    }

    if (r == 0)
    {
        r = getdns_list_set_dict(upstream_list, 0, server_dict);

        if (r != 0)
            fprintf(stderr, "Error: create upstream list");
    }

    if (r == 0)
    {
        r = getdns_context_set_upstream_recursive_servers(context,
            upstream_list);
    }

    if (r != 0)
        fprintf(stderr, "Error: set upstream");

    return r;
}


getdns_return_t GetContextForNamedTlsServer(getdns_context** tls_context, char* tls_auth_name)
{
    getdns_return_t      r;
    getdns_context * local_context = NULL;
    getdns_dict         *extensions = NULL;
    getdns_dict *response = NULL;


    /* Use the locally configured server to get the IP address of the TLS server */
    if ((r = getdns_context_create(&local_context, 1)))
        fprintf(stderr, "Creation of the local context failed, r = %x", r);
    else if (r = getdns_address_sync(local_context, tls_auth_name, extensions, &response))
        fprintf(stderr, "Resolution of %s in the local context failed, r = %x", 
            tls_auth_name, r);
    else if (r = InitializeGetDNSContext(tls_context))
        fprintf(stderr, "Creation of the TLS context failed, r = %x", r);
    else if (r = InitializeServerAddress(*tls_context, response, 853, tls_auth_name))
        fprintf(stderr, "Setting of the TLS upstream failed, r = %x", r);

    if (response != NULL)
        getdns_dict_destroy(response);

    if (local_context != NULL)
        getdns_context_destroy(local_context);
//#define LOTS_OF_DEBUG_INFO
#ifdef LOTS_OF_DEBUG_INFO
    if (r == 0)
    {
        getdns_dict * dict = getdns_context_get_api_information(*tls_context);
        if (dict)
        {
            fprintf(stderr, "%s\n", getdns_pretty_print_dict(dict));
            getdns_dict_destroy(dict);
        }
    }
#endif

    return r;
}

getdns_return_t SyncQuery(getdns_context *context, char *tls_auth_name
    , char *query_name, FILE* FRecords)
{
    getdns_return_t     r = 0;
    getdns_dict         *extensions = NULL;
    getdns_dict         *response = NULL;
    uint32_t            status;
    getdns_bindata      *address_data;
    char                *first = NULL;
    uint64_t start_time_microsec = _getdns_get_time_as_uintt64();
    uint64_t end_time_microsec, microsec_spent;
    uint32_t start_time_sec;

    r = getdns_address_sync(context, query_name, extensions, &response);

    end_time_microsec = _getdns_get_time_as_uintt64();
    microsec_spent = end_time_microsec - start_time_microsec;
    start_time_sec = (uint32_t)time(0);

    if (r == 0)
    {

        if ((r = getdns_dict_get_int(response, "status", &status)))
        {
            fprintf(stderr, "Could not get \"status\" from reponse for <%s>\n", query_name);
            r = GETDNS_RETURN_GENERIC_ERROR;
        }

        else if (status != GETDNS_RESPSTATUS_GOOD)
        {
            fprintf(stderr, "The search <%s> had no results, and a return value of %"PRIu32".\n",
                query_name, status);
            r = GETDNS_RETURN_BAD_DOMAIN_NAME;
        }

        else if ((r = getdns_dict_get_bindata(response, "/just_address_answers/0/address_data", &address_data)))
        {
            r = GETDNS_RETURN_BAD_DOMAIN_NAME;
            fprintf(stderr, "Could not get first address for <%s>\n", query_name);
        }

        else if (!(first = getdns_display_ip_address(address_data)))
        {
            fprintf(stderr, "Could not convert first address to string for <%s>\n",
                query_name);
            r = GETDNS_RETURN_BAD_DOMAIN_NAME;
        }
    }

    if (r == 0)
    {
        fprintf(FRecords, "%s, %s, 0, %d, %s, %d\n", tls_auth_name, query_name,
            (int)microsec_spent, first, start_time_sec);
    }
    else
    {
        fprintf(FRecords, "%s, %s, %d, %d, 0.0.0.0, %d\n",
            tls_auth_name, query_name, r, (int) microsec_spent, start_time_sec);
    }

    if (first != NULL)
    {
        free(first);
    }

    if (response != NULL)
    {
        getdns_dict_destroy(response);
    }

    return r;
}

getdns_return_t GetNameList(getdns_context *context, char *tls_auth_name, char * fname)
{
    FILE* F;
    char query_name[256];
    getdns_return_t r = 0, rq;

    F = fopen(fname, "r");

    if (F == NULL)
    {
        fprintf(stderr, "Cannot open <%s>\n", fname);
        r = GETDNS_RETURN_GENERIC_ERROR;
    }
    else
    {
        while (fgets(query_name, 256, F))
        {
            int l = strlen(query_name);
            while (l > 0 &&
                (query_name[l - 1] <= 32 || query_name[l - 1] >= 127))
            {
                l--;
                query_name[l] = 0;
            }

            rq = SyncQuery(context, (tls_auth_name)? tls_auth_name:"local", query_name, stdout);
            if (rq != 0)
            {
                r = rq;
            }
        }

        fclose(F);
    }

    return r;
}

getdns_return_t ServerTest(char *tls_auth_name, char * fname)
{
    getdns_return_t      r;  /* Holder for all function returns */
    getdns_context      *context = NULL;
    getdns_namespace_t myNameSpaces[] = {
        GETDNS_NAMESPACE_LOCALNAMES ,
        GETDNS_NAMESPACE_MDNS,
        GETDNS_NAMESPACE_DNS
    };
    size_t myNameSpacesNb = sizeof(myNameSpaces) / sizeof(getdns_namespace_t);

    if (tls_auth_name != NULL)
    {
        r = GetContextForNamedTlsServer(&context, tls_auth_name);
    }
    else
    {
        tls_auth_name = "local";
        if ((r = getdns_context_create(&context, 1)))
            fprintf(stderr, "Trying to create the local context failed, r = %x", r);
        else if ((r = getdns_context_set_namespaces(context, myNameSpacesNb, myNameSpaces)))
            fprintf(stderr, "Trying to set the local name space failed, r = %x", r);
    }

    if (r == 0)
    {
        r = GetNameList(context, tls_auth_name, fname);
    }

    if (context)
        getdns_context_destroy(context);

    return r;

}

getdns_return_t GetServerList(char *servers_file, char * fname)
{
    FILE* F;
    char server_name[256];
    getdns_return_t r = 0, rq;

    r = ServerTest(0, fname);

    F = fopen(servers_file, "r");

    if (F == NULL)
    {
        fprintf(stderr, "Cannot open <%s>\n", fname);
        r = GETDNS_RETURN_GENERIC_ERROR;
    }
    else
    {
        while (fgets(server_name, 256, F))
        {
            int l = strlen(server_name);
            while (l > 0 &&
                (server_name[l - 1] <= 32 || server_name[l - 1] >= 127))
            {
                l--;
                server_name[l] = 0;
            }

            rq = ServerTest(server_name, fname);

            if (rq != 0)
            {
                r = rq;
            }
        }

        fclose(F);
    }

    return r;
}


int main()
{
    getdns_return_t r = GetServerList("servernames.txt", "namelist.txt");

    exit(r);
}

#if 0
int main()
{
    getdns_return_t      r;  /* Holder for all function returns */
    getdns_context      *context = NULL;
    getdns_dict         *extensions = NULL;
    char                *fname = "namelist.txt";
    char                *query_name = "www.microsoft.com";
    /* Could add things here to help identify this call */
    char                *userarg = query_name;
    getdns_namespace_t myNameSpaces[] = {
        GETDNS_NAMESPACE_LOCALNAMES ,
        GETDNS_NAMESPACE_MDNS,
        GETDNS_NAMESPACE_DNS
    };
    size_t myNameSpacesNb = sizeof(myNameSpaces) / sizeof(getdns_namespace_t);
    char * tls_auth_name = "getdnsapi.net";/* "dnsovertls.sinodun.com" */;

    if (tls_auth_name != NULL)
    {
        r = GetContextForNamedTlsServer(&context, tls_auth_name);
    }
    else
    {
        if ((r = getdns_context_create(&context, 1)))
            fprintf(stderr, "Trying to create the context failed, r = %x", r);
        else if ((r = getdns_context_set_namespaces(context, myNameSpacesNb, myNameSpaces)))
            fprintf(stderr, "Trying to set the name space failed, r = %x", r);
    }

    if (r == 0)
    {
#ifdef ASYNC
        getdns_transaction_t transaction_id;
        if ((r = getdns_address(context, query_name, extensions
            , userarg, &transaction_id, callback)))
            fprintf(stderr, "Error scheduling asynchronous request");

        else {
            printf("Request with transaction ID %"PRIu64" scheduled.\n", transaction_id);

            getdns_context_run(context);
        }
#else
        {
#ifndef PSEUDO_SYNC
            if (fname != NULL)
            {
                r = GetNameList(context, tls_auth_name, fname);
            }
            else
            {
                r = SyncQuery(context, tls_auth_name, query_name);
            }
#else
            struct pseudo_sync_arg pa;

            r = pseudo_sync_call(context, query_name, &pa);

            if (r == 0)
            {
                getdns_bindata  address_data;
                char * first;

                address_data.size = pa.size;
                address_data.data = pa.addr;

                if (!(first = getdns_display_ip_address(&address_data)))
                    fprintf(stderr, "Could not convert first address to string\n");
                else
                {
                    printf("For %s, got address: %s\n", query_name, first);
                }
            }
#endif
        }
    }
#endif
	/* Clean up */

	if (context)
		getdns_context_destroy(context);

	/* Assuming we get here, leave gracefully */
	exit(EXIT_SUCCESS);
}
#endif
