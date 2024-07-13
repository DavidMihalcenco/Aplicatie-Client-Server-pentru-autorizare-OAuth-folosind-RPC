struct AccessRequest {
    string user_id<50>;
    string authorization_token<100>;
    int refresh;
    int valabil;
};

struct AccessResponse {
    string response_type<100>;
    string access_token<100>; 
    string refresh_token<100>; 
    int validity_period;
};

struct DelegatedActionRequest {
    string action<100>;
    string resource<100>;
    string access_token<100>;
    int autoRefresh;
};

struct DelegatedActionResponse{
    string response<100>;
    string new_acc_token<100>;
    string new_refr_token<100>;
};

program OauthAuthorization {
    version OauthVersion {
        string request_authorization(string) = 1;
        AccessResponse request_token(AccessRequest) = 2;
        DelegatedActionResponse validate_action(DelegatedActionRequest) = 3;
        string approve_token(string) = 4;
    } = 1;
} = 1; 