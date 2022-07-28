#include "client.h"

void myCSS_log(void);

int flag_end = 0;
t_callback_log dt_login;

int flag_window = 1;

void myCSS_signup(void);
void myCSS_log(void) {
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *myCssFile = "resource/css/login.css";
    GError *error = 0;

    gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
    g_object_unref(provider);
}

static int signin(SSL *ssl, char *nickname, char *pass, char *Nickname) {
    char *hash;
    char *returned;

    SSL_write(ssl, "SIGNIN", 7);

    SSL_write(ssl, nickname, strlen(nickname)); /* encrypt & send message */

    hash = hashing(pass);
    SSL_write(ssl, hash, strlen(hash)); /* encrypt & send message */

    returned = ssl_get_string(ssl);
    if (strncmp(returned, "FOUND", 6) == 0) {
        strcpy(Nickname, nickname);
        return 1;
    }

    else
        return 0; /* not found */
}

static int signup(SSL *ssl, char *username, char *nickname, char *pass, char *Nickname) {
    char *hash;

    SSL_write(ssl, "SIGNUP", 7);

    SSL_write(ssl, username, strlen(username)); /* encrypt & send message */

    SSL_write(ssl, nickname, strlen(nickname)); /* encrypt & send message */
    strcpy(Nickname, nickname);

    hash = hashing(pass);
    SSL_write(ssl, hash, strlen(hash)); /* encrypt & send message */

    return 1;
}

GtkWidget *window;

void start_reg() {
    const char *get_input_nick = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)dt_login.nick_entery));
    const char *get_input_login = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)dt_login.login_entery));
    const char *get_input_pass = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)dt_login.pass_entery));

    dt_login.input_nick = mx_strdup(get_input_nick);
    dt_login.input_login = mx_strdup(get_input_login);
    dt_login.input_pass = mx_strdup(get_input_pass);

    gtk_widget_destroy(window);
    gtk_main_quit();
}

void creat_register_wind() {
    //(void)param;

    GtkBuilder *builder;
    GtkWidget *login_entery;
    GtkWidget *name_entery;
    GtkWidget *password_entery;
    GtkWidget *enter_button;
    myCSS_signup();

    builder = gtk_builder_new_from_file("resource/glade/reg_window2.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "reg_window"));
    gtk_widget_set_name(window, "www");
    login_entery = GTK_WIDGET(gtk_builder_get_object(builder, "input_reg_login"));
    gtk_widget_set_name(login_entery, "login_entery");
    name_entery = GTK_WIDGET(gtk_builder_get_object(builder, "input_reg_name"));
    gtk_widget_set_name(name_entery, "name_entery");
    password_entery = GTK_WIDGET(gtk_builder_get_object(builder, "input_reg_pass"));
    gtk_widget_set_name(password_entery, "password_entery");
    enter_button = GTK_WIDGET(gtk_builder_get_object(builder, "reg_button"));
    gtk_widget_set_name(enter_button, "enter_button");

    dt_login.nick_entery = login_entery;
    dt_login.login_entery = name_entery;
    dt_login.pass_entery = password_entery;

    g_signal_connect(G_OBJECT(enter_button), "clicked", G_CALLBACK(start_reg), NULL);

    g_object_unref(builder);

    gtk_widget_show(window);

    gtk_main();
    // return param;
}

void start_log() {
    const char *get_input_login = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)dt_login.login_entery));
    const char *get_input_pass = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)dt_login.pass_entery));

    dt_login.input_login = mx_strdup(get_input_login);
    dt_login.input_pass = mx_strdup(get_input_pass);

    gtk_widget_destroy(window);
    gtk_main_quit();
}

void move_wind() {
    gtk_widget_destroy(window);
    flag_window = 0;
    creat_register_wind();
    gtk_main_quit();
}

void creat_login_wind() {
    //(void)param;

    GtkBuilder *builder;
    // GtkWidget *send_button;// *test_button;
    GtkWidget *login_entery;
    GtkWidget *password_entery;
    GtkWidget *enter_button;
    GtkWidget *move_to_register_button;
    myCSS_log();
    builder = gtk_builder_new_from_file("resource/glade/enter_window-2.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "log_window"));
    gtk_widget_set_name(window, "www");
    login_entery = GTK_WIDGET(gtk_builder_get_object(builder, "input_log_login"));
    gtk_widget_set_name(login_entery, "login_entery");
    password_entery = GTK_WIDGET(gtk_builder_get_object(builder, "input_log_pass"));
    gtk_widget_set_name(password_entery, "password_entery");
    enter_button = GTK_WIDGET(gtk_builder_get_object(builder, "enter_button"));
    gtk_widget_set_name(enter_button, "enter_button");
    move_to_register_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "move_to_register_button"));
    gtk_widget_set_name(move_to_register_button, "move_to_register_button");

    gtk_entry_set_visibility(GTK_ENTRY(password_entery), FALSE);

    dt_login.login_entery = login_entery;
    dt_login.pass_entery = password_entery;

    g_signal_connect(G_OBJECT(enter_button), "clicked", G_CALLBACK(start_log), NULL);
    g_signal_connect(G_OBJECT(move_to_register_button), "clicked", G_CALLBACK(move_wind), NULL);

    g_object_unref(builder);

    gtk_widget_show(window);

    gtk_main();

    // return param;
}

void myCSS_signup(void) {
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *myCssFile = "resource/css/signup.css";
    GError *error = 0;

    gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
    g_object_unref(provider);
}

int login(SSL *ssl, char *Nickname) {
    // char *buffer;
    dt_login.ssl = ssl;
    SSL_write(ssl, "LOGIN", 6);


    creat_login_wind();
    if (flag_window == 1) {
        // signin(ssl, dt_login.input_login, dt_login.input_pass);
        if (signin(ssl, dt_login.input_login, dt_login.input_pass, Nickname)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (signup(ssl, dt_login.input_nick, dt_login.input_login, dt_login.input_pass, Nickname)) {
            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}
