// $Id$

/**
 * Nodequeue object
 *
 * Settings:
 *
 * container
 * up
 * down
 * top
 * bottom: 
 * delete: input image
 * order: input hidden
 * add: input to add items
 * path: path to add items
 * tr: the base id of the tr
 */
Drupal.nodequeue = function(base, settings) {
  // Set the properties for this object.
  if (settings.container == null) {
    settings.container = 'tr';
  }

  var max = function(array) {
    return Math.max.apply(Math, array);
  };
  var maxPosition = max($(settings.order).val().split(','));

  this.settings = settings;
  this.base = '#' + base;

  // helper function to swap items in an array
  var swap = function(array, a, b) {
    var temp = array[a];
    array[a] = array[b];
    array[b] = temp;
  }

  var changeOrder = function(item, how) {
    var order = $(settings.order).val().split(',');

    if (how != 'add') {
      var tr = $(item).parents('tr').get(0);
      var id = $(tr).attr('id').replace(settings.tr, '');
      var index = -1;
    
      for (var i in order) {
        if (order[i] == id) {
          index = parseInt(i);
          break;
        }
      }
    }

    switch (how) {
      case 'add':
        order.push(item);
        break;
      case 'delete':
        order.splice(index, 1);
        break;
      case 'top':
        var temp = order[index];
        order.splice(index, 1);
        order.unshift(temp);
        break;
      case 'up':
        swap(order, index, index - 1);
        break;
      case 'down':
        swap(order, index, index + 1);
        break;
      case 'bottom':
        var temp = order[index];
        order.splice(index, 1);
        order.push(temp);
        break;
    }

    var new_order = '';
    for (i in order) {
      if (new_order) {
        new_order += ',';
      }
      new_order += order[i];
    }

    $(settings.order).val(new_order);
    $(settings.hidden).show();
  }

  this.changeOrder = changeOrder;

  var restripeTable = function(table) {
    // :even and :odd are reversed because jquery counts from 0 and
    // we count from 1, so we're out of sync.
    $('tbody tr:not(:hidden)', $(table))
      .removeClass('even')
      .removeClass('odd')
      .filter(':even')
        .addClass('odd')
      .end()
      .filter(':odd')
        .addClass('even');
  }

  this.restripeTable = restripeTable;

  // Set as a function so we can be both a closure and called later
  // when more items get added.
  var bindButtons = function() {
    if (settings.remove) {
      $(settings.remove + ':not(.nodequeue-processed)')
        .addClass('nodequeue-processed')
        .click(function() { return false; })
        .click(function(e) {
          var item = $(this).parents(settings.container);
          changeOrder(this, 'delete');

          item.remove();
          restripeTable('#' + base);

          return false;
        });
    }

    if (settings.up) {
      $(settings.up + ':not(.nodequeue-processed)')
        .addClass('nodequeue-processed')
        .click(function() { return false; })
        .click(function(e) {
          var item = $(this).parents(settings.container);
          var prev = $(item).prev();

          if (prev.html() != null && item != prev) {
            // move item
            prev.before(item);
            restripeTable('#' + base);
            changeOrder(this, 'up');
          }

          return false;
        });
    }

    if (settings.top) {
      $(settings.top + ':not(.nodequeue-processed)')
        .addClass('nodequeue-processed')
        .click(function() { return false; })
        .click(function(e) {
          var item = $(this).parents(settings.container);
          var first = $(item).siblings(':first');

          first.before(item);
          restripeTable('#' + base);
          changeOrder(this, 'top');

          return false;
        });
    }

    if (settings.down) {
      $(settings.down + ':not(.nodequeue-processed)')
        .addClass('nodequeue-processed')
        .click(function() { return false; })
        .click(function(e) {
          var item = $(this).parents(settings.container);
          var next = $(item).next();

          if (next.html() != null && item != next) {
            // move item
            next.after(item);
            restripeTable('#' + base);
            changeOrder(this, 'down');
          }

          return false;
        });
    }

    if (settings.bottom) {
      $(settings.bottom + ':not(.nodequeue-processed)')
        .addClass('nodequeue-processed')
        .click(function() { return false; })
        .click(function(e) {
          var item = $(this).parents(settings.container);
          var last = $(item).siblings(':last');
          
          // move item
          last.after(item);
          restripeTable('#' + base);
          changeOrder(this, 'bottom');

          return false;
        });
    }
    if (settings.add) {
      $(settings.add + ':not(.nodequeue-processed)')
        .addClass('nodequeue-processed')
        .click(function() { return false; })
        .click(function(e) {
          var input = this;
          $(input).attr('disabled', true);
          $(input).parent().addClass('throbbing');
          var data = { position: maxPosition + 1 };
          for (i in settings.post) {
            data[$(settings.post[i]).attr('name')] = $(settings.post[i]).val();
          }

          $.ajax({
            type: 'POST',
            data: data,
            url: settings.path,
            global: true,
            success: function (data) {
              // Parse response
              $(input).parent().removeClass('throbbing');
              $(input).attr('disabled', false);
              data = Drupal.parseJson(data);
              if (data.status) {
                // add new data
                var new_content = $(data.data);

                Drupal.freezeHeight();
                // Hide the new content before adding to page.

                // Add the form and re-attach behavior.
                $('#' + base + ' tbody').append(new_content);

                maxPosition++;
                changeOrder(maxPosition, 'add');
                bindButtons();

                Drupal.unfreezeHeight();

                // Add the extra data, if necessary
                if (data.extra && settings.extra) {
                  var val = $(settings.extra).val();
                  if (val) {
                    val += ',';
                  }
                  val += data.extra;
                  $(settings.extra).val(val);
                }

                if (settings.clear) {
                  for (i in settings.clear) {
                    $(settings.clear[i]).val('');
                  }
                }
                return;
              }
              else {
                // report the error
                alert(data.error, 'Error');
              }
            },
            error: function(data) {
              alert('An error occurred');
              $(input).parent().removeClass('throbbing');
              $(input).attr('disabled', false);
            }
          });
          return false;
        });
    }
  }

  // Focus if we're told to.
  if (settings.focus) {
    $(settings.focus).get(0).focus();
  }
  this.bindButtons = bindButtons();
}


Drupal.nodequeue.autoAttach = function() {
  if (Drupal.settings && Drupal.settings.nodequeue) {
    for (var base in Drupal.settings.nodequeue) {
      if (!$('#'+ base + '.nodequeue-processed').size()) {
        var settings = Drupal.settings.nodequeue[base];
        var list = new Drupal.nodequeue(base, settings);
        $('#'+ base).addClass('nodequeue-processed');
      }
    }
  }

  $('a.nodequeue-ajax-toggle').click(function() {
    var a = this;
    href = $(this).attr('href');
    $(a).addClass('throbbing');
    $.ajax({
      type: 'POST',
      data: { js: 1 },
      url: href,
      global: true,
      success: function (data) {
        // Parse response
        $(a).removeClass('throbbing');
        data = Drupal.parseJson(data);
        // Change text on success
        if (data.status) {
          // Change label back
          $(a).attr('href', data.href);
          $(a).html(data.label);
          if (data.sqid) {
            $('#nodequeue-count-' + data.sqid).html(data.count);
          }
          return;
        }
      },
      error: function(data) {
        $(a).removeClass('throbbing');
      }
    });
    return false;
  });

}

if (Drupal.jsEnabled) {
  $(document).ready(Drupal.nodequeue.autoAttach);
}



