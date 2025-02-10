(function ($, once) {
  Backdrop.behaviors.downloadCountExport = {
    attach: function (context) {
      once('download-count-export-toggle', '#download-count-export-form', context).forEach(function (form) {
        $(form).find('div.form-item-download-count-export-date-range-from').hide();
        $(form).find('div.form-item-download-count-export-date-range-to').hide();

        $('input#edit-download-count-export-range-0', form).on('click', function () {
          $(form).find('div.form-item-download-count-export-date-range-from').hide();
          $(form).find('div.form-item-download-count-export-date-range-to').hide();
        });

        $('input#edit-download-count-export-range-1', form).on('click', function () {
          $(form).find('div.form-item-download-count-export-date-range-from').show();
          $(form).find('div.form-item-download-count-export-date-range-to').show();
        });
      });
    }
  };
})(jQuery, once);
