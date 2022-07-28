#include "client.h"

static bool check_pop = false;
static bool check_chose = false;

t_callback dt_run;
GtkAdjustment *vAdjust;
GtkWidget *label;
GtkWidget *hbox;
GtkWidget *vbox;
GtkWidget *u_name;
GtkWidget *c_button;
GtkWidget *friendsTreeView;
GtkWidget *popup_w;
GtkWidget *choser;

GtkWidget *image;

void set_chat();
// GtkRequisition req;

static void ShowCerts(SSL *ssl) {
    X509 *cert;

    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert == NULL)
        printf("Info: No client certificates configured.\n");
}

void sleep_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void chose_photo() {   
    if(!check_chose) {    
        gtk_window_present( GTK_WINDOW( choser ) );
    }
    else {
        gtk_widget_hide(choser);
    }
    check_chose = !check_chose; 
    //gtk_window_present( GTK_WINDOW( choser ) );
}


void open_file() {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(choser));
    SSL_write(dt_run.ssl, filename, strlen(filename));
    g_print("%s", filename);
}



void show_img(int side, char *path) {
    //int side =1;
    const GdkPixbuf *pix = gdk_pixbuf_new_from_file(path, NULL);
    //image = gtk_image_new_from_pixbuf(gdk_pixbuf_copy(pixbuf));
    int a = gdk_pixbuf_get_width(pix), b = gdk_pixbuf_get_height(pix);
    g_object_unref(G_OBJECT(pix));
    int old_b = b;
    if(b > 200) b = 200;
    a = b * a / old_b;

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(path, a, b, FALSE, NULL);
    //gdk_pixbuf_scale_simple(GDK_PIXBUF(pixbuf), a, b, GDK_INTERP_BILINEAR);

    image = gtk_image_new_from_pixbuf(gdk_pixbuf_copy(pixbuf));
    gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 10);
    if (side == 1) {
        gtk_widget_set_halign(image, GTK_ALIGN_END);
    } else {
        gtk_widget_set_halign(image, GTK_ALIGN_START);
    }
    gtk_widget_show(image);
}

void print_window(char *string, int side) {
    label = gtk_label_new(NULL);

    gtk_widget_set_name(label, "text");

    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 10);
    

    if (side == 1) {
        gtk_widget_set_halign(label, GTK_ALIGN_END);
    } else {
        gtk_widget_set_halign(label, GTK_ALIGN_START);
    }

    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD_CHAR);
    //
    gtk_label_set_markup(GTK_LABEL(label), mx_format_text(string));

    g_object_ref(label);
    gtk_widget_show(vbox);
    gtk_widget_show(label);
}

void start_chat_with_user(GtkWidget *widget, __attribute__((unused)) gpointer *data) {
    GList *children, *iter;

    children = gtk_container_get_children(GTK_CONTAINER(vbox));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);

    dt_run.input_usr =
        mx_strjoin("ENTER PERSONAL ", mx_strdup(gtk_button_get_label(GTK_BUTTON(widget))));

    g_print("%s", dt_run.input_usr);

    SSL_write(dt_run.ssl, dt_run.input_usr, strlen(dt_run.input_usr));
    g_object_set(G_OBJECT(u_name), "text", gtk_button_get_label(GTK_BUTTON(widget)), NULL);
    dt_run.flag = 1;
    if (!strncmp(&dt_run.input_usr[6], "OVERVIEW", 9)) {
        dt_run.VIEW = VIEW_OVERVIEW;
        SSL_write(dt_run.ssl, "RECEIVE OVERVIEW", 17);
    } else {
        dt_run.VIEW = VIEW_NYI;
        SSL_write(dt_run.ssl, "RECEIVE", 8);
    }
}

void search_event() {
    const char *get_input = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)dt_run.search_entery));
    c_button = gtk_button_new_with_label(get_input);
    gtk_widget_set_name(c_button, "c_button");
    gtk_box_pack_start(GTK_BOX(friendsTreeView), c_button, FALSE, FALSE, 3);
    gtk_widget_show(c_button);

    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)dt_run.search_entery), "");
    g_signal_connect(G_OBJECT(c_button), "clicked", G_CALLBACK(start_chat_with_user), NULL);
}

void writer() {
    const char *get_input = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)dt_run.message_entery));

    dt_run.input = mx_strdup(get_input);

    g_print("MESSAGE TO SERVER: %s\n", dt_run.input);
    if (!strncmp(dt_run.input, "RECEIVE", 7)) { /* if receive, then start read */
        dt_run.flag = 1;
        if (!strncmp(&dt_run.input[8], "OVERVIEW", 9)) {
            dt_run.VIEW = VIEW_OVERVIEW;
        } else
            dt_run.VIEW = VIEW_NYI;
    }
    SSL_write(dt_run.ssl, dt_run.input, strlen(dt_run.input));
    if (!strncmp(dt_run.input, "CREATE ", 7)) { /* if create, then get image*/
        char *picture;

        picture = strtok(&dt_run.input[7], " ");
        send_picture(dt_run.ssl, picture);
    }
    if (!strncmp(dt_run.input, "ENTER ", 6)) {
        dt_run.flag = 1;
        if (!strncmp(&dt_run.input[6], "OVERVIEW", 9)) {
            dt_run.VIEW = VIEW_OVERVIEW;
            SSL_write(dt_run.ssl, "RECEIVE OVERVIEW", 17);
        } else {
            dt_run.VIEW = VIEW_NYI;
            SSL_write(dt_run.ssl, "RECEIVE", 8);
        }
    }

    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)dt_run.message_entery), "");
}

void myCSS(void) {
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *myCssFile = "resource/css/theme.css";
    GError *error = 0;

    gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
    g_object_unref(provider);
}

static int reader(SSL *ssl) { /*if it returns 0 we stop reading*/
    char *username;
    char *message;
    char *respondent;
    char *dates;
    int hours;
    int minutes;

    if (dt_run.VIEW == VIEW_OVERVIEW) {
        return overview(ssl);
    }

    username = ssl_get_string(ssl);
    if (!mx_strncmp(username, "EOF", 4)) {
        return 0;
    }

    message = ssl_get_string(ssl);

    respondent = ssl_get_string(ssl);

    dates = ssl_get_string(ssl);
    hours = (strtol(dates, NULL, 10) / 3600) % 24 + 2;
    minutes = (strtol(dates, NULL, 10) / 60) % 60;

    if (strncmp(respondent, "|||ZERO|||", 11) == 0) {
        // print_window(message);
        printf("%s - %s @%d:%d\n", username, message, hours, minutes);
        // print_on_screen(username, message, NULL);
        vAdjust = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(hbox));
        if (!strcmp(username, dt_run.Nickname)) {
            if (mx_count_substr(message, ".png") == 0 && mx_count_substr(message, ".jpg") == 0) {
                print_window(message, 1);
            }
            else {
                show_img(1, message);
            }
            sleep_ms(10);
            double value = gtk_adjustment_get_upper(vAdjust);

            gtk_adjustment_set_value(vAdjust, value);   
            
        } else {
            if (mx_count_substr(message, ".png") == 0 && mx_count_substr(message, ".jpg") == 0) {
                print_window(message, 0);
            }
            else {
                show_img(0, message);
                
            }
            sleep_ms(10);
            double value = gtk_adjustment_get_upper(vAdjust);

            gtk_adjustment_set_value(vAdjust, value);
            
        }

    }

    else
        printf("%s - %s @%d:%d re: %s\n", username, message, hours, minutes, respondent);
    fflush(stdout);

    free(username);
    free(message);
    free(respondent);
    free(dates);
    return 1;
}

void *creat_wind(void *param) {
    (void)param;

    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *send_button;
    GtkWidget *send_entery;
    GtkWidget *doc_button;
    GtkWidget *search_button;
    GtkWidget *search_user_button;
    GtkWidget *setting_button;
    GtkWidget *name_popup_label;
    GtkWidget *profile_name_text;
    GtkWidget *log_out_button;

    myCSS();
    builder = gtk_builder_new_from_file("resource/glade/protest.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    
    gtk_widget_set_name(window, "ddd");

    u_name = GTK_WIDGET(gtk_builder_get_object(builder, "show_user_name"));

    send_button = GTK_WIDGET(gtk_builder_get_object(builder, "message_send_button"));
    gtk_widget_set_name(send_button, "send_button");
    send_entery = GTK_WIDGET(gtk_builder_get_object(builder, "message_button"));
    gtk_widget_set_name(send_entery, "send_entery");
    doc_button = GTK_WIDGET(gtk_builder_get_object(builder, "doc_button"));
    gtk_widget_set_name(doc_button, "doc_button");
    search_button = GTK_WIDGET(gtk_builder_get_object(builder, "search_button"));
    gtk_widget_set_name(search_button, "search_button");
    search_user_button = GTK_WIDGET(gtk_builder_get_object(builder, "search_user_button"));
    gtk_widget_set_name(search_user_button, "search_user_button");
    friendsTreeView = GTK_WIDGET(gtk_builder_get_object(builder, "friendsTreeView"));
    gtk_widget_set_name(friendsTreeView, "friendsTreeView");
    setting_button = GTK_WIDGET(gtk_builder_get_object(builder, "setting_button"));
    gtk_widget_set_name(setting_button, "setting_button");
    gtk_widget_set_name(u_name, "u_name");
    name_popup_label = GTK_WIDGET(gtk_builder_get_object(builder, "name_popup_label"));
    gtk_widget_set_name(name_popup_label, "name_popup_label");
    profile_name_text = GTK_WIDGET(gtk_builder_get_object(builder, "profile_name_text"));
    gtk_widget_set_name(profile_name_text, "profile_name_text");
    log_out_button = GTK_WIDGET(gtk_builder_get_object(builder, "log_out_button"));
    gtk_widget_set_name(log_out_button, "log_out_button");
    name_popup_label = GTK_WIDGET(gtk_builder_get_object(builder, "name_popup_label"));
    popup_w = GTK_WIDGET( gtk_builder_get_object( builder, "popup_w" ) );
    gtk_builder_connect_signals( builder, popup_w );

    choser = GTK_WIDGET( gtk_builder_get_object( builder, "choser" ) );

    GObject *btn = gtk_builder_get_object(builder,
                                          "btn_filechooser_open");

    g_signal_connect(btn, "clicked", G_CALLBACK(open_file), NULL);

    gtk_label_set_text(GTK_LABEL(name_popup_label), dt_run.Nickname);

    dt_run.message_entery = send_entery;
    dt_run.search_entery = search_button;

    hbox = GTK_WIDGET(gtk_builder_get_object(builder, "box"));
    gtk_widget_set_name(hbox, "boxx");
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_container_add(GTK_CONTAINER(hbox), vbox);

    g_signal_connect(G_OBJECT(send_button), "clicked", G_CALLBACK(writer), NULL);
    g_signal_connect(G_OBJECT(send_entery), "activate", G_CALLBACK(writer), NULL);
    g_signal_connect(G_OBJECT(search_user_button), "clicked", G_CALLBACK(search_event), NULL);
    g_signal_connect(G_OBJECT(setting_button), "clicked", G_CALLBACK(set_chat), NULL);
    g_signal_connect(G_OBJECT(doc_button), "clicked", G_CALLBACK(chose_photo), NULL);
    
    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();
    return param;
}

void set_chat() {
    if(!check_pop) {    
        gtk_window_present( GTK_WINDOW( popup_w ) );
    }
    else {
        gtk_widget_hide(popup_w);
    }
    check_pop = !check_pop;
}

void *updater(void *ssl) {
    while (ssl) {
        sleep(2);
        if (dt_run.VIEW != VIEW_OVERVIEW) {
            dt_run.flag = true;
            SSL_write(dt_run.ssl, "RECEIVE", 8);
        }
    }
    return NULL;
}

int run(SSL *ssl, char *Nickname) {
    pthread_t watcher;
    pthread_t updat;

    ShowCerts(ssl);
    dt_run.ssl = ssl;
    dt_run.VIEW = VIEW_OVERVIEW;
    dt_run.flag = false;
    dt_run.Nickname = Nickname;

    pthread_create(&watcher, 0, creat_wind, 0);
    pthread_create(&updat, 0, updater, ssl);

    // SSL_write(dt_run.ssl, "RECEIVE OVERVIEW", 17);
    while (true) {
        if (dt_run.flag) {
            dt_run.flag = false;
            while (reader(ssl))
                ;
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl); /* release connection state */
    return 0;
}
